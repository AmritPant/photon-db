#include "../../include/request-handler/xread-command-handler.h"
#include "../../include/stream-store.h"
#include "../../include/resp-parser.h"
#include <cctype>
#include <chrono>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

static std::string upper_copy(std::string s) {
    for (auto &c : s) c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
    return s;
}

// Caller must hold get_streams_mutex(). Returns "" if nothing new to report.
static std::string build_reply(const std::vector<std::string> &keys,
                                const std::vector<StreamEntryId> &start_ids) {
    auto &streams = get_streams();
    std::vector<std::pair<const std::string *, std::vector<const StreamEntry *>>> results;

    for (size_t i = 0; i < keys.size(); i++) {
        auto it = streams.find(keys[i]);
        if (it == streams.end()) continue;

        std::vector<const StreamEntry *> matched;
        for (const auto &e : it->second.entries) {
            if (id_greater(e.id, start_ids[i])) {
                matched.push_back(&e);
            }
        }
        if (!matched.empty()) {
            results.push_back({&keys[i], std::move(matched)});
        }
    }

    if (results.empty()) return "";

    std::string resp = "*" + std::to_string(results.size()) + "\r\n";
    for (auto &pair : results) {
        resp += "*2\r\n";
        resp += bulk_string_resp(*pair.first);
        resp += "*" + std::to_string(pair.second.size()) + "\r\n";
        for (const auto *e : pair.second) {
            resp += "*2\r\n";
            resp += bulk_string_resp(format_id(e->id));
            resp += "*" + std::to_string(e->fields.size() * 2) + "\r\n";
            for (const auto &f : e->fields) {
                resp += bulk_string_resp(f.first);
                resp += bulk_string_resp(f.second);
            }
        }
    }
    return resp;
}

std::string xread_command_handler(const std::vector<std::string> &command_array) {
    long long block_ms = -1; // -1 => no BLOCK option given
    size_t idx = 1;

    while (idx < command_array.size()) {
        std::string token = upper_copy(command_array[idx]);
        if (token == "BLOCK") {
            if (idx + 1 >= command_array.size()) {
                return "-ERR syntax error\r\n";
            }
            try {
                block_ms = std::stoll(command_array[idx + 1]);
            } catch (...) {
                return "-ERR value is not an integer or out of range\r\n";
            }
            idx += 2;
        } else if (token == "COUNT") {
            if (idx + 1 >= command_array.size()) {
                return "-ERR syntax error\r\n";
            }
            idx += 2; // accepted but not enforced
        } else if (token == "STREAMS") {
            idx += 1;
            break;
        } else {
            return "-ERR syntax error\r\n";
        }
    }

    if (idx > command_array.size()) {
        return "-ERR syntax error\r\n";
    }

    size_t remaining = command_array.size() - idx;
    if (remaining == 0 || remaining % 2 != 0) {
        return "-ERR Unbalanced XREAD list of streams: for each stream key an ID or '$' must be specified.\r\n";
    }

    size_t n = remaining / 2;
    std::vector<std::string> keys(command_array.begin() + idx, command_array.begin() + idx + n);
    std::vector<std::string> id_strs(command_array.begin() + idx + n, command_array.begin() + idx + remaining);
    std::vector<StreamEntryId> start_ids(n);

    {
        std::lock_guard<std::mutex> lock(get_streams_mutex());
        auto &streams = get_streams();
        for (size_t i = 0; i < n; i++) {
            if (id_strs[i] == "$") {
                auto it = streams.find(keys[i]);
                start_ids[i] = (it != streams.end()) ? it->second.last_id : StreamEntryId{0, 0};
            } else if (!parse_full_id(id_strs[i], start_ids[i])) {
                // Accept a bare number (e.g. "0") as shorthand for "0-0"
                try {
                    start_ids[i] = {std::stoull(id_strs[i]), 0};
                } catch (...) {
                    return "-ERR Invalid stream ID specified as stream command argument\r\n";
                }
            }
        }
    }

    std::string reply;
    {
        std::lock_guard<std::mutex> lock(get_streams_mutex());
        reply = build_reply(keys, start_ids);
    }
    if (!reply.empty()) return reply;

    if (block_ms < 0) {
        return "*-1\r\n"; // no BLOCK requested, nothing new right now
    }

    // --- Blocking path ---
    // IMPORTANT: this server currently serves clients from a single-threaded
    // poll() loop (see src/main.cpp). Sleeping/polling here pauses that whole
    // loop, so a second client's XADD will not be processed while this call
    // is waiting. This becomes correct once the server is made
    // multi-threaded (Amrit's Week 1 "Handle concurrent clients" stage).
    // Until then, this will only behave correctly in single-connection tests
    // (e.g. XADD happens before BLOCK is issued, or timeout-based tests).
    bool forever = (block_ms == 0);
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(block_ms);

    while (forever || std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        std::lock_guard<std::mutex> lock(get_streams_mutex());
        reply = build_reply(keys, start_ids);
        if (!reply.empty()) return reply;
    }

    return "*-1\r\n"; // timed out with nothing new
}

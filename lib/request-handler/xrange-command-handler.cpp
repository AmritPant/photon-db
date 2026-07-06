#include "../../include/request-handler/xrange-command-handler.h"
#include "../../include/stream-store.h"
#include "../../include/resp-parser.h"
#include <mutex>
#include <vector>
#include <limits>

static bool parse_range_id(const std::string &s, bool is_start, StreamEntryId &out) {
    if (s == "-") { out = {0, 0}; return true; }
    if (s == "+") { out = {UINT64_MAX, UINT64_MAX}; return true; }

    size_t dash = s.find('-');
    try {
        if (dash == std::string::npos) {
            out.ms = std::stoull(s);
            out.seq = is_start ? 0ULL : UINT64_MAX;
        } else {
            out.ms = std::stoull(s.substr(0, dash));
            out.seq = std::stoull(s.substr(dash + 1));
        }
    } catch (...) {
        return false;
    }
    return true;
}

std::string xrange_command_handler(const std::string &key, const std::string &start, const std::string &end) {
    StreamEntryId start_id, end_id;
    if (!parse_range_id(start, true, start_id) || !parse_range_id(end, false, end_id)) {
        return "-ERR Invalid stream ID specified as stream command argument\r\n";
    }

    std::lock_guard<std::mutex> lock(get_streams_mutex());
    auto &streams = get_streams();
    auto it = streams.find(key);

    if (it == streams.end()) {
        return "*0\r\n";
    }

    std::vector<const StreamEntry *> matched;
    for (const auto &e : it->second.entries) {
        if (id_greater_or_equal(e.id, start_id) && id_greater_or_equal(end_id, e.id)) {
            matched.push_back(&e);
        }
    }

    std::string resp = "*" + std::to_string(matched.size()) + "\r\n";
    for (const auto *e : matched) {
        resp += "*2\r\n";
        resp += bulk_string_resp(format_id(e->id));
        resp += "*" + std::to_string(e->fields.size() * 2) + "\r\n";
        for (const auto &f : e->fields) {
            resp += bulk_string_resp(f.first);
            resp += bulk_string_resp(f.second);
        }
    }
    return resp;
}

#include "../../include/request-handler/xadd-command-handler.h"
#include "../../include/stream-store.h"
#include "../../include/resp-parser.h"
#include <chrono>
#include <mutex>

static std::string error_resp(const std::string &msg) {
    return "-ERR " + msg + "\r\n";
}

std::string xadd_command_handler(const std::vector<std::string> &command_array) {
    // XADD key id field value [field value ...]
    if (command_array.size() < 5 || (command_array.size() - 3) % 2 != 0) {
        return error_resp("wrong number of arguments for 'xadd' command");
    }

    const std::string &key = command_array[1];
    const std::string &raw_id = command_array[2];

    std::lock_guard<std::mutex> lock(get_streams_mutex());
    auto &streams = get_streams();
    auto &stream = streams[key]; // creates entry if missing

    bool stream_has_history = !stream.entries.empty() ||
        stream.last_id.ms != 0 || stream.last_id.seq != 0;

    StreamEntryId new_id{};

    if (raw_id == "*") {
        uint64_t now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        new_id.ms = now_ms;
        if (stream_has_history && now_ms == stream.last_id.ms) {
            new_id.seq = stream.last_id.seq + 1;
        } else {
            new_id.seq = (now_ms == 0) ? 1 : 0;
        }
    } else {
        size_t dash = raw_id.find('-');
        if (dash != std::string::npos && raw_id.substr(dash + 1) == "*") {
            // Partially auto-generated: "ms-*"
            try {
                new_id.ms = std::stoull(raw_id.substr(0, dash));
            } catch (...) {
                return error_resp("Invalid stream ID specified as stream command argument");
            }
            if (stream_has_history && new_id.ms == stream.last_id.ms) {
                new_id.seq = stream.last_id.seq + 1;
            } else {
                new_id.seq = (new_id.ms == 0) ? 1 : 0;
            }
        } else {
            if (!parse_full_id(raw_id, new_id)) {
                return error_resp("Invalid stream ID specified as stream command argument");
            }
        }
    }

    if (new_id.ms == 0 && new_id.seq == 0) {
        return error_resp("The ID specified in XADD must be greater than 0-0");
    }

    if (stream_has_history && !id_greater(new_id, stream.last_id)) {
        return error_resp("The ID specified in XADD is equal or smaller than the target stream top item");
    }

    StreamEntry entry;
    entry.id = new_id;
    for (size_t i = 3; i + 1 < command_array.size(); i += 2) {
        entry.fields.emplace_back(command_array[i], command_array[i + 1]);
    }

    stream.entries.push_back(entry);
    stream.last_id = new_id;

    return bulk_string_resp(format_id(new_id));
}

#include "../include/stream-store.h"

std::unordered_map<std::string, StreamData> &get_streams() {
    static std::unordered_map<std::string, StreamData> streams;
    return streams;
}

std::mutex &get_streams_mutex() {
    static std::mutex m;
    return m;
}

std::string format_id(const StreamEntryId &id) {
    return std::to_string(id.ms) + "-" + std::to_string(id.seq);
}

bool id_greater(const StreamEntryId &a, const StreamEntryId &b) {
    if (a.ms != b.ms) return a.ms > b.ms;
    return a.seq > b.seq;
}

bool id_greater_or_equal(const StreamEntryId &a, const StreamEntryId &b) {
    return id_greater(a, b) || (a.ms == b.ms && a.seq == b.seq);
}

bool parse_full_id(const std::string &id_str, StreamEntryId &out) {
    size_t dash = id_str.find('-');
    if (dash == std::string::npos) return false;
    try {
        out.ms = std::stoull(id_str.substr(0, dash));
        out.seq = std::stoull(id_str.substr(dash + 1));
    } catch (...) {
        return false;
    }
    return true;
}

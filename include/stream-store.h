#pragma once
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <mutex>
#include <cstdint>
#include <climits>

struct StreamEntryId {
    uint64_t ms = 0;
    uint64_t seq = 0;
};

struct StreamEntry {
    StreamEntryId id;
    std::vector<std::pair<std::string, std::string>> fields;
};

struct StreamData {
    std::vector<StreamEntry> entries;
    StreamEntryId last_id{0, 0};
};

std::unordered_map<std::string, StreamData> &get_streams();
std::mutex &get_streams_mutex();

std::string format_id(const StreamEntryId &id);
bool parse_full_id(const std::string &id_str, StreamEntryId &out);
bool id_greater(const StreamEntryId &a, const StreamEntryId &b);
bool id_greater_or_equal(const StreamEntryId &a, const StreamEntryId &b);

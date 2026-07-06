#pragma once
#include <chrono>
#include <optional>
#include <string>
#include <unordered_map>

struct Entry {
    std::string value;
    std::optional<std::chrono::steady_clock::time_point> expiry;
};

std::unordered_map<std::string, Entry> &get_store();
std::unordered_map<std::string, std::vector<std::string>> &get_lists();
bool is_expired(const Entry &entry);

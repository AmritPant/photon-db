#include "../include/store.h"

std::unordered_map<std::string, Entry> &get_store(){
    static std::unordered_map<std::string, Entry> store;
    return store;
}

std::unordered_map<std::string, std::vector<std::string>> &get_lists() {
    static std::unordered_map<std::string, std::vector<std::string>> lists;
    return lists;
}

bool is_expired(const Entry &entry){
    if (!entry.expiry.has_value()) {
        return false; // No expiry set, so it's not expired
    }
    auto now = std::chrono::steady_clock::now();
    return now >= entry.expiry.value();
}
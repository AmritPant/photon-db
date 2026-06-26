#include "../include/data-store.h"
#include <unordered_map>

namespace {
std::unordered_map<std::string, std::string> key_type_store;
std::unordered_map<std::string, std::string> key_string_store;
}

void set_key_type(const std::string &key, const std::string &type) {
    key_type_store[key] = type;
}

std::string get_key_type(const std::string &key) {
    auto entry = key_type_store.find(key);
    if (entry == key_type_store.end()) {
        return "none";
    }

    return entry->second;
}

void set_string_value(const std::string &key, const std::string &value) {
    key_string_store[key] = value;
    set_key_type(key, "string");
}

bool get_string_value(const std::string &key, std::string &value) {
    auto entry = key_string_store.find(key);
    if (entry == key_string_store.end()) {
        return false;
    }

    value = entry->second;
    return true;
}

int delete_key(const std::string &key) {
    int deleted_count = 0;

    auto type_erased = key_type_store.erase(key);
    auto value_erased = key_string_store.erase(key);

    if (type_erased > 0 || value_erased > 0) {
        deleted_count = 1;
    }

    return deleted_count;
}

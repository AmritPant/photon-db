#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <optional>
#include <cstdint>

struct RdbEntry {
    std::string key;
    std::string value;
    std::optional<uint64_t> expiry_ms;
};

class RdbParser {
private:
    std::ifstream file;

    uint8_t read_uint8();
    uint64_t read_uint64_le();
    uint32_t read_uint32_be();
    std::pair<uint32_t, bool> read_length();

public:
    explicit RdbParser(const std::filesystem::path& path);
    void parse_header();
    std::string read_string();
    std::vector<RdbEntry> parse_db_entries();
}; 
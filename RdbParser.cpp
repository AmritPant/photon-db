#include "RdbParser.hpp"
#include <stdexcept>

RdbParser::RdbParser(const std::filesystem::path& path) {
    file.open(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open RDB file: " + path.string());
    }
}

uint8_t RdbParser::read_uint8() {
    uint8_t val;
    if (!file.read(reinterpret_cast<char*>(&val), 1)) throw std::runtime_error("Unexpected EOF");
    return val;
}

uint64_t RdbParser::read_uint64_le() {
    uint64_t val;
    if (!file.read(reinterpret_cast<char*>(&val), 8)) throw std::runtime_error("Unexpected EOF");
    return val;
}

uint32_t RdbParser::read_uint32_be() {
    uint8_t bytes[4];
    if (!file.read(reinterpret_cast<char*>(bytes), 4)) throw std::runtime_error("Unexpected EOF");
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

std::pair<uint32_t, bool> RdbParser::read_length() {
    uint8_t first_byte = read_uint8();
    uint8_t enc_type = (first_byte & 0xC0) >> 6;

    if (enc_type == 00) return { first_byte & 0x3F, false };
    if (enc_type == 01) return { ((first_byte & 0x3F) << 8) | read_uint8(), false };
    if (enc_type == 10) return { read_uint32_be(), false };
    return { first_byte & 0x3F, true };
}

void RdbParser::parse_header() {
    char magic[5]; char version[4];
    if (!file.read(magic, 5) || std::string(magic, 5) != "REDIS") throw std::runtime_error("Invalid Magic");
    if (!file.read(version, 4)) throw std::runtime_error("Invalid Version");
}

std::string RdbParser::read_string() {
    auto [length_or_format, is_encoded] = read_length();
    if (!is_encoded) {
        std::string str(length_or_format, '\0');
        file.read(&str[0], length_or_format);
        return str;
    }
    if (length_or_format == 0) return std::to_string(static_cast<int8_t>(read_uint8()));
    if (length_or_format == 1) { int16_t val; file.read(reinterpret_cast<char*>(&val), 2); return std::to_string(val); }
    if (length_or_format == 2) { int32_t val; file.read(reinterpret_cast<char*>(&val), 4); return std::to_string(val); }
    throw std::runtime_error("Unsupported encoding context");
}

std::vector<RdbEntry> RdbParser::parse_db_entries() {
    std::vector<RdbEntry> entries;
    std::optional<uint64_t> current_expiry = std::nullopt;

    while (true) {
        uint8_t opcode = read_uint8();
        if (opcode == 0xFF) break; 
        if (opcode == 0xFA) { read_string(); read_string(); continue; }
        if (opcode == 0xFE) { read_length(); continue; }
        if (opcode == 0xFB) { read_length(); read_length(); continue; }

        if (opcode == 0xFC) {
            current_expiry = read_uint64_le();
            opcode = read_uint8();
        } else if (opcode == 0xFD) {
            uint32_t expiry_seconds = 0;
            file.read(reinterpret_cast<char*>(&expiry_seconds), 4);
            current_expiry = static_cast<uint64_t>(expiry_seconds) * 1000;
            opcode = read_uint8();
        }

        if (opcode == 0) { 
            std::string key = read_string();
            std::string value = read_string();
            entries.push_back({key, value, current_expiry});
            current_expiry = std::nullopt;
        }
    }
    return entries;
}
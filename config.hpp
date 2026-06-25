#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <filesystem>

struct Config {
    std::string dir = ".";
    std::string dbfilename = "dump.rdb";

    std::filesystem::path get_rdb_path() const;
};

Config parse_args(int argc, char* argv[]);

#endif // CONFIG_HPP
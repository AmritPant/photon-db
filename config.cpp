#include "config.hpp"
#include <vector>

std::filesystem::path Config::get_rdb_path() const {
    return std::filesystem::path(dir) / dbfilename;
}

Config parse_args(int argc, char* argv[]) {
    Config config;
    std::vector<std::string> args(argv + 1, argv + argc);

    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--dir" && i + 1 < args.size()) {
            config.dir = args[++i];
        } else if (args[i] == "--dbfilename" && i + 1 < args.size()) {
            config.dbfilename = args[++i];
        }
    }
    return config;
}
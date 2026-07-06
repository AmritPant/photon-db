#include "config.hpp"
#include <vector>

std::filesystem::path Config::get_rdb_path() const {
    return std::filesystem::path(dir); // dbfilename;
}

Config parse_args(int argc, char* argv[]) {
    Config config;
    std::vector<std::string> args(argv + 1, argv + argc);

    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--dir" && i + 1 < args.size()) {
            config.dir = args[++i];
        } 
        else if (args[i] == "--dbfilename" && i + 1 < args.size()) {
            config.dbfilename = args[++i];
        } 
        //Parse --appendonly <yes/no>
        else if (args[i] == "--appendonly" && i + 1 < args.size()) {
            std::string val = args[++i];
            config.aof.append_only = (val == "yes");
        } 
        //Parse --appendfilename <filename>
        else if (args[i] == "--appendfilename" && i + 1 < args.size()) {
            config.aof.append_filename = args[++i];
        }
    }
    return config;
}
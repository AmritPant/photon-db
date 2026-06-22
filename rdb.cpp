#include <iostream>
#include <string>
#include <vector>

struct Config {
    std::string dir = ".";
    std::string dbfilename = "demo.rdb";
};  //reads strings dir and dbfilename

int main(int argc, char* argv[]) {  //argc is the argument counter & argv is argument vector
    Config config;
    std::vector<std::string> args(argv, argv + argc);

    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--dir" && i + 1 < args.size()) {
            config.dir = args[++i];
        } else if (args[i] == "--dbfilename" && i + 1 < args.size()) {
            config.dbfilename = args[++i];
        }
    }

    // Quick verification
    std::cout << "Data Directory: " << config.dir << "\n";
    std::cout << "RDB Filename:   " << config.dbfilename << "\n";

    // Your server startup logic goes here...
    
    return 0;
}
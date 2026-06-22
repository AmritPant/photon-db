#include <iostream>
#include <string>
#include <vector>
#include <filesystem> // Useful for path handling

struct ServerConfig {
    std::string dir = ".";            // Default to current working directory
    std::string dbfilename = "dump.rdb"; // Default Redis RDB filename
};

ServerConfig parse_arguments(int argc, char* argv[]) {
    ServerConfig config;
    
    // Convert C-style arguments to a vector of std::string for safety
    std::vector<std::string> args(argv, argv + argc);

    // Start at index 1 because index 0 is always the executable path
    for (size_t i = 1; i < args.size(); ++i) {
        
        if (args[i] == "--dir") {
            // Check if there is a next element to read as the value
            if (i + 1 < args.size()) {
                config.dir = args[++i]; // Advance index and assign value
            } else {
                std::cerr << "Error: --dir requires a directory path argument.\n";
            }
        } 
        
        else if (args[i] == "--dbfilename") {
            // Check if there is a next element to read as the value
            if (i + 1 < args.size()) {
                config.dbfilename = args[++i]; // Advance index and assign value
            } else {
                std::cerr << "Error: --dbfilename requires a filename argument.\n";
            }
        }
    }

    return config;
}

int main(int argc, char* argv[]) {
    // 1. Parse configuration flags
    ServerConfig config = parse_arguments(argc, argv);

    // 2. Resolve the absolute or full relative path
    // Using C++17 <filesystem> makes path joining cross-platform and reliable
    std::filesystem::path base_dir(config.dir);
    std::filesystem::path file_name(config.dbfilename);
    std::filesystem::path full_rdb_path = base_dir / file_name; 

    // 3. Print out to verify during boot up
    std::cout << "[INFO] Configuration Loaded Successfully.\n";
    std::cout << "       Target Directory: " << config.dir << "\n";
    std::cout << "       Target Filename:  " << config.dbfilename << "\n";
    std::cout << "       Full Path to RDB: " << full_rdb_path.string() << "\n\n";

    // Next step: Pass 'full_rdb_path' to your RDB parsing subsystem...
    
    return 0;
}
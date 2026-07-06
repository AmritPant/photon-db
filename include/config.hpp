#pragma once

#include <string>
#include <filesystem>

struct AofConfig {
    /**
     * When enabled, the background AOF rewrite process will prepend an RDB
     * snapshot preamble to the .aof file for faster startup recovery.
     */
    bool use_rdb_preamble = true;          

    /**
     * If true, prevents the main process from calling fsync() while a 
     * background BGSAVE or BGREWRITEAOF is actively performing heavy disk I/O.
     */
    bool no_appendfsync_on_rewrite = false; 
    
    /**
     * Main toggle for Append Only File persistence.
     * Controlled via the --appendonly flag.
     */
    bool append_only = false;

    /**
     * The name of the persistent log file.
     * Controlled via the --appendfilename flag.
     */
    std::string append_filename = "appendonly.aof";
};

struct Config {
    // Port mapping
    int port = 6379;

    // RDB Storage Configurations
    std::string dir = ".";
    std::string dbfilename = "dump.rdb";

    // Nested AOF Sub-configurations
    AofConfig aof;

    /**
     * Computes and returns the full absolute or relative path to the RDB file
     * by joining the directory and the dbfilename.
     */
    std::filesystem::path get_rdb_path() const;
};

/**
 * Parses command-line arguments to override default configuration options.
 * Handles:
 * --dir <directory>
 * --dbfilename <filename>
 * --appendonly <yes/no>
 * --appendfilename <filename>
 */
Config parse_args(int argc, char* argv[]);
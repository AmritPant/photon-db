#include <iostream>
#include <string>

struct AofConfig {

  bool use_rdb_preamble = true;           // aof-use-rdb-preamble yes
  bool no_appendfsync_on_rewrite = false; // no-appendfsync-on-rewrite no

  bool append_only = false;
  std::string append_filename = "appendonly.aof"; // Name of persistent log file
};

struct ServerConfig {
  int port = 6379;
  AofConfig aof;
};

// Global configuration
ServerConfig g_config;

void print_current_aof_config() {
  std::cout << "--- PhotonDB AOF Configuration ---" << std::endl;
  std::cout << "aof-use-rdb-preamble: "
            << (g_config.aof.use_rdb_preamble ? "yes" : "no") << std::endl;
  std::cout << "no-appendfsync-on-rewrite: "
            << (g_config.aof.no_appendfsync_on_rewrite ? "yes" : "no")
            << std::endl;
}


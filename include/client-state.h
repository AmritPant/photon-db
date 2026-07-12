#pragma once
#include <string>
#include <vector>

struct ClientState {
    int fd = -1;                 
    std::string buffer;          

    bool in_multi = false;       // true after they send MULTI, until EXEC/DISCARD
    std::vector<std::vector<std::string>> tx_queue; // queued commands, each one is like {"SET","foo","bar"}
};
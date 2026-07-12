#include "../../include/request-handler/discard-command-handler.h"

std::string discard_command_handler(ClientState &client) {
    if (!client.in_multi) {
        return "-ERR DISCARD without MULTI\r\n";
    }

    client.in_multi = false;
    client.tx_queue.clear();

    return "+OK\r\n";
}
#include "../../include/request-handler/multi-command-handler.h"

std::string multi_command_handler(ClientState &client) {
    if(client.in_multi) {
        return "-ERR MULTI calls can not be nested\r\n";
    }
    client.in_multi = true;
    return "+OK\r\n";
}
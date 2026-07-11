#include "../../include/request-handler/exec-command-handler.h"

std::string exec_command_handler(ClientState &client) {
    if (!client.in_multi) {
        return "-ERR EXEC without MULTI\r\n";
    }

    client.in_multi = false;  

    return "*0\r\n";
}
#include "../../include/request-handler/exec-command-handler.h"

std::string exec_command_handler(ClientState &client) {
    if (!client.in_multi) {
        return "-ERR EXEC without MULTI\r\n";
    }

    client.in_multi = false;  

    std::string resp = "*" + std::to_string(client.tx_queue.size()) + "\r\n";

    for (auto &queued_command : client.tx_queue) {
        resp += dispatch_command(queued_command, client);
    }

    client.tx_queue.clear();
    return resp;
}
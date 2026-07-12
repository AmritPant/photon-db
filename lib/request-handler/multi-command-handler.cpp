#include "../../include/request-handler/multi-command-handler.h"

std::string multi_command_handler(ClientState &client) {
    client.in_multi = true;
    return "+OK\r\n";
}
#include "../include/request-router.h"
#include "../include/resp-parser.h"
#include "../include/store.h"
// Command Handler
#include "../include/request-handler/echo-command-handler.h"
#include "../include/request-handler/ping-command-handler.h"
#include "../include/request-handler/set-command-handler.h"
#include "../include/request-handler/get-command-handler.h"
#include "../include/request-handler/incr-command-handler.h"
#include "../include/request-handler/decr-command-handler.h"
#include "../include/request-handler/type-command-handler.h"
#include "../include/request-handler/xadd-command-handler.h"
#include "../include/request-handler/xrange-command-handler.h"
#include "../include/request-handler/xread-command-handler.h"
#include "../include/request-handler/multi-command-handler.h"
#include "../include/request-handler/exec-command-handler.h"
#include "../include/request-handler/discard-command-handler.h"

void upper(std::string &text) {
    for (int i = 0; i < text.length(); i++) {
        text[i] = toupper(text[i]);
    }
}

std::string dispatch_command(std::vector<std::string> &command_array, ClientState &client) {
    std::string resp;
    if (command_array[0] == "PING") {
        resp = ping_command_handler();
    } else if (command_array[0] == "ECHO") {
        resp = echo_command_handler(command_array[1]);
    } else if (command_array[0] == "COMMAND") {
        resp = "+PONG\r\n";
    } else if (command_array[0] == "MULTI") {
        resp = multi_command_handler(client);
    } else if (command_array[0] == "EXEC") {
        resp = exec_command_handler(client);
    } else if (command_array[0] == "DISCARD") {
    resp = discard_command_handler(client);
    } else if (command_array[0] == "SET") {
        if (command_array.size() > 4) {
            resp = set_command_handler(command_array[1], command_array[2], 
                command_array[3], command_array[4]);
        } else resp = set_command_handler(command_array[1], command_array[2]);
    } else if (command_array[0] == "GET") {
        resp = get_command_handler(command_array[1]);
    } else if (command_array[0] == "INCR") {
        resp = incr_command_handler(command_array[1]);
    } else if (command_array[0] == "DECR") {
        resp = decr_command_handler(command_array[1]);
    } else if (command_array[0] == "TYPE") {
        resp = type_command_handler(command_array[1]);
    } else if (command_array[0] == "XADD") {
        resp = xadd_command_handler(command_array);
    } else if (command_array[0] == "XRANGE") {
        if (command_array.size() < 4) {
            resp = "-ERR wrong number of arguments for 'xrange' command\r\n";
        } else {
            resp = xrange_command_handler(command_array[1], command_array[2], command_array[3]);
        }
    } else if (command_array[0] == "XREAD") {
        resp = xread_command_handler(command_array);
    } else {
        resp = "-ERR unknown command\r\n";
    }
    return resp;
}

std::string request_router(const char *buffer, ClientState &client) {
    std::vector<std::string> command_array;
    resp_to_text(buffer, command_array);
    upper(command_array[0]);

    if (client.in_multi &&
        command_array[0] != "MULTI" &&
        command_array[0] != "EXEC" &&
        command_array[0] != "DISCARD") {

        client.tx_queue.push_back(command_array);
        return "+QUEUED\r\n";
    }

    return dispatch_command(command_array, client);
}
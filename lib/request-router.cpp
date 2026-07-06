// system Header files
#include <cctype>
// User Defined Header files
#include "../include/request-router.h"
#include "../include/resp-parser.h"
#include "../include/store.h"
// Command Handler
#include "../include/request-handler/decr-command-handler.h"
#include "../include/request-handler/echo-command-handler.h"
#include "../include/request-handler/get-command-handler.h"
#include "../include/request-handler/incr-command-handler.h"
#include "../include/request-handler/ping-command-handler.h"
#include "../include/request-handler/set-command-handler.h"
#include "../include/request-handler/type-command-handler.h"

// List Command Handler
#include "../include/request-handler/list-commands/llen-command-handler.h"
#include "../include/request-handler/list-commands/lpop-command-handler.h"
#include "../include/request-handler/list-commands/lrange-command-handler.h"
#include "../include/request-handler/rpush-handler.h"

void upper(std::string &text) {
    for (int i = 0; i < text.length(); i++) {
        text[i] = toupper(text[i]);
    }
}
std::string request_router(const char *buffer) {
    std::vector<std::string> command_array;
    resp_to_text(buffer, command_array);

    if (command_array.empty()) {
        return "-ERR protocol error\r\n";
    }

    // Converts text into upper case
    upper(command_array[0]);

    // Repsonse text string since resp is string
    std::string resp;

    // Different Cases
    if (command_array[0] == "PING") {
        resp = ping_command_handler();
    } else if (command_array[0] == "ECHO") {
        resp = echo_command_handler(command_array[1]);
    } else if (command_array[0] == "COMMAND") {
        resp = "+PONG\r\n";
    } else if (command_array[0] == "SET") {
        if (command_array.size() > 4) {
            resp = set_command_handler(command_array[1], command_array[2], command_array[3], command_array[4]);
        } else
            resp = set_command_handler(command_array[1], command_array[2]);
    } else if (command_array[0] == "GET") {
        resp = get_command_handler(command_array[1]);
    } else if (command_array[0] == "INCR") {
        resp = incr_command_handler(command_array[1]);
    } else if (command_array[0] == "DECR") {
        resp = decr_command_handler(command_array[1]);
    } else if (command_array[0] == "TYPE") {
        resp = type_command_handler(command_array[1]);
    }
    // List commands
    // ----- RPUSH -----------
    // List Commands
    // Handlesh both "RPUSH" & "LPUSH"
    else if (command_array[0] == "RPUSH" || command_array[0] == "LPUSH") {
        return handle_rpush(command_array);
    } else if (command_array[0] == "LRANGE") {
        return lrange_command_handler(command_array);
    } else if (command_array[0] == "LLEN") {
        return llen_command_handler(command_array);
    } else if (command_array[0] == "LPOP") {
        return lpop_command_handler(command_array);
    } else {
        resp = "-ERR unknown command\r\n";
    }

    return resp;
}

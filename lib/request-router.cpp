// system Header files
#include <cctype>

// User Defined Header files
#include "../include/request-router.h"
#include "../include/resp-parser.h"

// Command Handler
#include "../include/request-handler/echo-command-handler.h"
#include "../include/request-handler/ping-command-handler.h"
#include "../include/request-handler/set-command-handler.h"
#include "../include/request-handler/get-command-handler.h"

void upper(std::string &text) {
    for (int i = 0; i < text.length(); i++) {
        text[i] = toupper(text[i]);
    }
}

std::string request_router(const char *buffer) {

    std::vector<std::string> command_array;
    resp_to_text(buffer, command_array);
    upper(command_array[0]);

    std::string resp;

    if (command_array[0] == "PING") {
        resp = ping_command_handler();
    } else if (command_array[0] == "ECHO") {
        resp = echo_command_handler(command_array[1]);
    } else if (command_array[0] == "COMMAND") {
        resp = "+PONG\r\n";
    }

    return resp;
}

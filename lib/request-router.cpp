// system Header files
#include <cctype>
#include <unordered_map>
#include <vector>

// User Defined Header files
#include "../include/request-router.h"
#include "../include/resp-parser.h"

// Commabnd Handler
#include "../include/request-handler/echo-command-handler.h"
#include "../include/request-handler/ping-command-handler.h"
#include "../include/request-handler/type-command-handler.h"

using CommandHandler = std::string (*)(const std::vector<std::string> &);

std::string ping_handler_adapter(const std::vector<std::string> &command_array) {
    if (command_array.size() != 1) {
        return "-ERR wrong number of arguments for 'ping' command\r\n";
    }

    return ping_command_handler();
}

std::string echo_handler_adapter(const std::vector<std::string> &command_array) {
    if (command_array.size() < 2) {
        return "-ERR wrong number of arguments for 'echo' command\r\n";
    }

    return echo_command_handler(command_array[1]);
}

std::string type_handler_adapter(const std::vector<std::string> &command_array) {
    if (command_array.size() < 2) {
        return "-ERR wrong number of arguments for 'type' command\r\n";
    }

    return type_command_handler(command_array[1]);
}

std::string command_handler_adapter(const std::vector<std::string> &command_array) {
    if (command_array.size() != 1) {
        return "-ERR wrong number of arguments for 'command' command\r\n";
    }

    return "+PONG\r\n";
}

const std::unordered_map<std::string, CommandHandler> command_handlers = {
    {"PING", ping_handler_adapter},
    {"ECHO", echo_handler_adapter},
    {"TYPE", type_handler_adapter},
    {"COMMAND", command_handler_adapter},
};

void upper(std::string &text) {
    for (int i = 0; i < text.length(); i++) {
        text[i] = toupper(text[i]);
    }
}

std::string request_router(const char *buffer) {

    std::vector<std::string> command_array;
    resp_to_text(buffer, command_array);
    if (command_array.empty()) {
        return "-ERR empty command\r\n";
    }

    upper(command_array[0]);

    auto matched_handler = command_handlers.find(command_array[0]);
    if (matched_handler == command_handlers.end()) {
        return "-ERR unknown command\r\n";
    }

    return matched_handler->second(command_array);
}

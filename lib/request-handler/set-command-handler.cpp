#include "../include/request-handler/set-command-handler.h"
#include "../include/store.h"

std::string set_command_handler(const std::string &key, const std::string &value) {
    auto &store = get_store();
    store[key] = value;
    return text_to_resp("OK");
}
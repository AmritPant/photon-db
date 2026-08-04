#include "../../include/request-handler/type-command-handler.h"
#include "../../include/store.h"

std::string type_command_handler(const std::string& key) {
    // Check string store first
    auto& store = get_store();
    auto it = store.find(key);
    if (it != store.end() && !is_expired(it->second)) {
        return "+string\r\n";
    }

    // Check list store
    auto& lists = get_lists();
    auto lit = lists.find(key);
    if (lit != lists.end()) {
        return "+list\r\n";
    }

    // Key doesn't exist in any store
    return "+none\r\n";
}
#include "../../include/request-handler/type-command-handler.h"
#include "../../include/store.h"

std::string type_command_handler(const std::string& key) {
    auto& store = get_store();
    auto it = store.find(key);

    if (it == store.end() || is_expired(it->second)) {
        return "+none\r\n";
    }

    return "+string\r\n";
}

#include "../../include/request-handler/type-command-handler.h"
#include "../../include/store.h"
#include "../../include/stream-store.h"

std::string type_command_handler(const std::string& key) {
    auto& store = get_store();
    auto it = store.find(key);

    if (it != store.end() && !is_expired(it->second)) {
        return "+string\r\n";
    }

    auto &streams = get_streams();
    if (streams.find(key) != streams.end()) {
        return "+stream\r\n";
    }

    return "+none\r\n";
}

#include "../../include/request-handler/get-command-handler.h"
#include "../../include/store.h"
#include "../../include/resp-parser.h"

std::string get_command_handler(const std::string &key) {
    auto &store = get_store();
    auto it = store.find(key);
    if (it==store.end()) {
        return "$-1\r\n"; // RESP Null Bulk String
    }
    if(is_expired(it->second)) {
        store.erase(it);
        return "$-1\r\n"; // RESP Null Bulk String
    }
    return bulk_string_resp(it->second.value); // RESP Bulk String
}
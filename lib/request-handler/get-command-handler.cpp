#include "../../include/request-handler/get-command-handler.h"
#include "../../include/store.h"
#include "../../include/resp-parser.h"

std::string get_command_handler(const std::string &key) {
    auto &store = get_store();
    auto it = store.find(key);
    if (it != store.end()) {
        return bulk_string_resp(it->second); // RESP Bulk String
    } else {
        return "$-1\r\n"; // RESP Null Bulk String
    }


}
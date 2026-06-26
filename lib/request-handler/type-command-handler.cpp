#include "../../include/request-handler/type-command-handler.h"
#include "../../include/data-store.h"
#include "../../include/resp-parser.h"

std::string type_command_handler(const std::string &key) {
    return text_to_resp(get_key_type(key));
}

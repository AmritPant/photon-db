#include "../../include/request-handler/incr-command-handler.h"
#include "../../include/store.h"
#include "../../include/request-handler/set-command-handler.h"
#include "../../include/request-router.h"
#include "../../include/resp-parser.h"

std::string incr_command_handler(const std::string &key) {
    auto &store = get_store();
    auto it = store.find(key);

    try {
        if (it==store.end() || is_expired(it->second)) {
            set_command_handler(key, "1"); // Set the key to 1 if it doesn't exist
            return integer_resp(1); // RESP Integer
        }
        if (std::isspace(it->second.value[0])) {
            return "-ERR value is not an integer or out of range\r\n"; // RESP Error
        }
    
        std::size_t processed_chars=0;
        long long value = std::stoll(it->second.value, &processed_chars);
        if (processed_chars != it->second.value.size()|| value == std::numeric_limits<long long>::max()) {
            return "-ERR value is not an integer or out of range\r\n"; // RESP Error
        }

        value++;
        it->second.value = std::to_string(value);
        return longlong_resp(value); // RESP Long Long Integer
    }
    catch (const std::invalid_argument& e) {
        return "-ERR value is not an integer or out of range\r\n"; // RESP Error
    }
    catch (const std::out_of_range& e) {
        return "-ERR value is not an integer or out of range\r\n"; // RESP Error
    }
    
}
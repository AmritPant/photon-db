
// User defined functions
#include "../../../include/request-handler/list-commands/lpop-command-handler.h"
#include "../../../include/resp-parser.h"
#include "../../../include/store.h"

// libarary functions
#include <unordered_map>

std::string lpop_command_handler(const std::vector<std::string> &command_array) {

    // Get the lists
    std::unordered_map<std::string, std::vector<std::string>> &lists = get_lists();

    // Get the key
    std::string key = command_array[1];

    // Returning a bulk empty string when no key is matched
    if (lists.find(key) == lists.end()) {
        std::string empty_string;
        std::string resp = bulk_string_resp(empty_string);
        return resp;
    }

    std::vector<std::string> &list = lists[key];

    if (command_array.size() == 2) {
        std::string removed = list.at(0);

        if (!list.empty()) {
            list.erase(list.begin());
        }
        std::string resp = text_to_resp(removed);
        return resp;
    } else if (command_array.size() == 3) {
        int num = std::stoi(command_array[2]);

        std::vector<std::string> removed_vector(list.begin(), list.begin() + num);
        list.erase(list.begin(), list.begin() + num);

        std::string resp = array_to_resp(removed_vector);
        return resp;
    } else {
        return text_to_resp("invalid command");
    }
}

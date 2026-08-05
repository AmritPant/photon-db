// User defined functions
#include "../../../include/request-handler/list-commands/lpop-command-handler.h"
#include "../../../include/resp-parser.h"
#include "../../../include/store.h"
// library functions
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>   // std::min
#include <stdexcept>   // std::invalid_argument, std::out_of_range

std::string lpop_command_handler(const std::vector<std::string> &command_array) {
    // Arity check: LPOP needs a key, and optionally a count -> 2 or 3 elements
    if (command_array.size() < 2 || command_array.size() > 3) {
        return "-ERR wrong number of arguments for 'lpop' command\r\n";
    }

    std::unordered_map<std::string, std::vector<std::string>> &lists = get_lists();
    const std::string &key = command_array[1];

    // Missing key -> null reply (canonical RESP nil bulk string)
    auto it = lists.find(key);
    if (it == lists.end()) {
        return "$-1\r\n";
    }

    std::vector<std::string> &list = it->second;

    // Existing-but-empty key -> also null; and clean up the dangling key
    if (list.empty()) {
        lists.erase(it);
        return "$-1\r\n";
    }

    // ---- LPOP key  (single element) ----
    if (command_array.size() == 2) {
        std::string removed = list.front();
        list.erase(list.begin());

        if (list.empty()) {
            lists.erase(it);   // Redis deletes the key when the list empties
        }
        return bulk_string_resp(removed);
    }

    // ---- LPOP key N  (count) ----
    // command_array.size() == 3 here
    int num = 0;
    try {
        num = std::stoi(command_array[2]);
    } catch (const std::exception &) {
        return "-ERR value is not an integer or out of range\r\n";
    }

    if (num < 0) {
        return "-ERR value is out of range, must be positive\r\n";
    }

    // Clamp so we never walk past the end of the vector
    num = std::min(num, static_cast<int>(list.size()));

    std::vector<std::string> removed_vector(list.begin(), list.begin() + num);
    list.erase(list.begin(), list.begin() + num);

    if (list.empty()) {
        lists.erase(it);
    }
    return array_to_resp(removed_vector);
}
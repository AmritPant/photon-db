// User defined
#include "../../include/request-handler/rpush-handler.h"
#include "../../include/resp-parser.h" // wherever longlong_resp lives on your team
#include "../../include/store.h"
// Libraries
#include <unordered_map>
#include <vector>

std::string handle_rpush(std::vector<std::string> command_array) {
    if (command_array.size() < 3) {
        return "-ERR wrong number of arguments for 'rpush' command\r\n";
    }

    std::string key = command_array[1];
    std::unordered_map<std::string, std::vector<std::string>> &lists = get_lists();

    // Inserting values into the list
    if (command_array[0] == "RPUSH") {
        // RPUSH appends to the tail: RPUSH key a b c -> [a, b, c]
        for (size_t i = 2; i < command_array.size(); i++) {
            lists[key].push_back(command_array[i]);
        }
    } else if (command_array[0] == "LPUSH") {
        // LPUSH prepends to the head: LPUSH key a b c -> [c, b, a]
        for (size_t i = 2; i < command_array.size(); i++) {
            lists[key].insert(lists[key].begin(), command_array[i]);
        }
    }

    // Number of elements in the list after the push
    long long size = lists[key].size();

    // Convert to RESP integer format
    std::string response_resp = longlong_resp(size);
    return response_resp;
}
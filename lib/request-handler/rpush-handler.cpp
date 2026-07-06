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

    // Inserting Value Inside the lists
    if (command_array[0] == "RPUSH") {
        for (size_t i = 2; i < command_array.size(); i++) {
            lists[key].push_back(command_array[i]);
        }
    } else if (command_array[0] == "LPUSH") {
        for (size_t i = (command_array.size() - 1); i > 1; i--) {
            lists[key].push_back(command_array[i]);
        }
    }

    // Getting number of elements in String
    // std::string response_string = std::to_string(lists[key].size());
    long long size = lists[key].size();

    // Converting that String into Resp Format
    std::string response_resp = longlong_resp(size);

    return response_resp;
}

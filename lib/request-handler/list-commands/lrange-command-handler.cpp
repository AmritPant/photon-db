// User defined Functions
#include "../../../include/request-handler/list-commands/lrange-command-handler.h"
#include "../../../include/resp-parser.h"
#include "../../../include/store.h"
// Library
#include "unordered_map"

std::string lrange_command_handler(std::vector<std::string> command_array) {
    // (0) Arity check — LRANGE requires exactly: key start stop
    if (command_array.size() != 4) {
        return "-ERR wrong number of arguments for 'lrange' command\r\n";
    }

    // Getting the list from the store
    std::unordered_map<std::string, std::vector<std::string>> &lists = get_lists();
    std::string list_name = command_array[1];
    std::vector<std::string> empty_array;
    std::string resp;

    // (1) If list doesn't exist, return an empty array
    if (lists.find(list_name) == lists.end()) {
        resp = array_to_resp(empty_array);
        return resp;
    }

    // Parse start/stop; return an error if they aren't integers
    int start, end;
    try {
        start = std::stoi(command_array[2]);
        end = std::stoi(command_array[3]);
    } catch (...) {
        return "-ERR value is not an integer or out of range\r\n";
    }

    std::vector<std::string> list = lists[list_name];
    int len = static_cast<int>(list.size());

    // Handling negative indexes
    if (start < 0) start = len + start;
    if (end < 0) end = len + end;

    // Clamp to valid bounds
    if (start < 0) start = 0;
    if (end >= len) end = len - 1;

    // Empty range
    if (start > end || start >= len) {
        resp = array_to_resp(empty_array);
        return resp;
    }

    std::vector<std::string> temp_array;
    for (int i = start; i <= end; i++) {
        temp_array.push_back(list.at(i));
    }
    resp = array_to_resp(temp_array);
    return resp;
}
// User defined Functions
#include "../../../include/request-handler/list-commands/lrange-command-handler.h"
#include "../../../include/resp-parser.h"
#include "../../../include/store.h"

// Library
#include "unordered_map"

std::string lrange_command_handler(std::vector<std::string> command_array) {
    // Getting the List from the store
    std::unordered_map<std::string, std::vector<std::string>> &lists = get_lists();

    // Name of the List
    std::string list_name = command_array[1];

    // Creating one empty array
    std::vector<std::string> empty_array;

    // Creating a response
    std::string resp;

    // (1) If list doesn't exist return an empty array
    if (lists.find(list_name) == lists.end()) {
        resp = array_to_resp(empty_array);
        return resp;
    }

    // (2) if start index is greater then or equal the lis's length an empty array is returned
    int start = std::stoi(command_array[2]);
    int end = std::stoi(command_array[3]);

    // getting list
    std::vector<std::string> list = lists[command_array[1]];

    // Handling negative indexes
    if (start < 0) {
        start = list.size() + start;
    }

    if (end < 0) {
        end = list.size() + end;
    }

    if (start > end || start == end || start > list.size()) {
        resp = array_to_resp(empty_array);
        return resp;
    }

    // assigning the last if end is greater then the whole value
    if (end > list.size()) {
        end = list.size();
    }

    std::vector<std::string> temp_array;

    for (int i = start; i <= end; i++) {
        temp_array.push_back(list.at(i));
    }

    resp = array_to_resp(temp_array);
    return resp;
}

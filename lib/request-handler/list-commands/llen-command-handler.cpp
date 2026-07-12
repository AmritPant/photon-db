
// user Defined Header
#include "../../../include/request-handler/list-commands/llen-command-handler.h"
#include "../../../include/resp-parser.h"
#include "../../../include/store.h"

// Library include
std::string llen_command_handler(const std::vector<std::string> &command_array) {

    // Getting the key
    std::string key = command_array[1];

    // Load the store
    std::unordered_map<std::string, std::vector<std::string>> &lists = get_lists();

    // Get the list
    std::vector<std::string> list = lists[key];

    // Getting the size
    long long size = list.size();

    // Converting to resp;
    std::string resp;
    resp = longlong_resp(size);

    // Returning the size
    return resp;
}

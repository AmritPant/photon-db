
// rpush-handler.cpp
#include "../../include/request-handler/rpush-handler.h"
#include "../../include/store.h"
#include "../../include/resp-parser.h"   // wherever longlong_resp lives on your team
#include<mutex>
std::string handle_rpush(std::vector<std::string> command_array) {
    if (command_array.size() < 3) {
        return "-ERR wrong number of arguments for 'rpush' command\r\n";
    }

    std::string key = command_array[1];
    size_t new_length = 0;

    {
        std::lock_guard<std::mutex> clock(get_store.mtx);
        auto &target_list = get_store(key);   // creates an empty deque if key is new — stage 1!

        for (size_t i = 2; i < command_array.size(); i++) {
            target_list.push_back(command_array[i]); // stages 2 & 3: same loop either way
        }
        new_length = target_list.size();
    } // mutex auto-unlocks here

   // get_store.cv.notify_all(); // wake any BLPOP threads waiting on this key
    return longlong_resp(new_length);
    
}
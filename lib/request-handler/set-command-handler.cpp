#include "../../include/request-handler/set-command-handler.h"
#include "../../include/store.h"
#include "../../include/resp-parser.h"
#include "../../include/request-router.h"

std::string set_command_handler(const std::string &key, const std::string &value, std::optional<std::string> flag, std::optional<std::string> number){
    auto &store = get_store();
    std::optional<std::chrono::steady_clock::time_point> expiry_time = std::nullopt;
    if (flag.has_value()) {
        upper(flag.value());
        long long duration = std::stol(number.value());
        if(flag.value()=="PX") expiry_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(duration);
        if(flag.value()=="EX") expiry_time = std::chrono::steady_clock::now() + std::chrono::seconds(duration);
    }
    store[key] = Entry{value, expiry_time};
    return text_to_resp("OK");
}
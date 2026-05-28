#include "../../include/request-handler/echo-command-handler.h"
#include "../../include/resp-parser.h"

std::string echo_command_handler(const std::string message){
  return text_to_resp(message);
}

#include "../../include/request-handler/ping-command-handler.h"
#include "../../include/resp-parser.h"

std::string ping_command_handler() { return text_to_resp("PONG"); }

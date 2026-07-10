#include <iostream>

// system Header files
#include <cctype>
// User Defined Header files
#include "resp-parser.h"
#include "store.h"
#include "client-state.h"
// Command Handler
#include "request-handler/echo-command-handler.h"
#include "request-handler/ping-command-handler.h"
#include "request-handler/set-command-handler.h"
#include "request-handler/get-command-handler.h"
#include "request-handler/incr-command-handler.h"
#include "request-handler/decr-command-handler.h"
#include "request-handler/type-command-handler.h"
#include "request-handler/multi-command-handler.h"

void upper(std::string &text);
std::string request_router(const char *buffer, ClientState &client);
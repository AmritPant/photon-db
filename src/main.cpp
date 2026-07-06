// System Header Files
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <netdb.h>
#include <unordered_map>
#include <vector>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// User Header Files
#include "../include/request-router.h"
#include "../include/resp-parser.h"
#include "aof.hpp"      // <-- Pull in AOF Subsystem Manager
#include "store.h"    // <-- Pull in get_store() reference mappings

int main(int argc, char **argv) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // --- 1. Week 3 Task: Parse command-line args for persistence flags ---
    bool appendOnlyEnabled = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--appendonly" && i + 1 < argc) {
            if (std::string(argv[i + 1]) == "yes") {
                appendOnlyEnabled = true;
            }
        }
    }

    // --- 2. Week 3 Task: Initialize and start AOF subsystem ---
    AOFManager::getInstance().initialize(appendOnlyEnabled);
    if (!AOFManager::getInstance().start()) {
        std::cerr << "Failed to initialize AOF subsystem infrastructure.\n";
        return 1;
    }

    // --- 3. Week 3 Task: Replay log files to restore memory state on startup ---
    AOFManager::getInstance().replayLog();

    // 1) Creating A Socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Failed to create server socket\n";
        return 1;
    }

    // Ensures that we don't run into 'Address already in use' errors
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        std::cerr << "setsockopt failed\n";
        return 1;
    }

    // 2) Binding a Socket
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(6379);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        std::cerr << "Failed to bind to port 6379\n";
        return 1;
    }

    // 3) Listening to the Socket
    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0) {
        std::cerr << "listen failed\n";
        return 1;
    }

    // Creating an array of Polls
    std::vector<struct pollfd> pagers;
    pagers.push_back({server_fd, POLLIN, 0});
    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    std::cout << "Waiting for a client to connect...\n";
    std::unordered_map<int, std::string> client_buffers;

    while (true) {
        // Blocking until first request arrives
        poll(pagers.data(), pagers.size(), -1);
        for (int i = pagers.size() - 1; i >= 0; i--) {
            if (pagers[i].revents & POLLIN) {
                if (pagers[i].fd == server_fd) {
                    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
                    std::cout << "Client connected\n";
                    pagers.push_back({client_fd, POLLIN, 0});
                } else {
                    // Receiving the message
                    int client_fd = pagers[i].fd;
                    char buffer[1024] = {0}; // Clear initialization buffer
                    int bytes_received = recv(client_fd, (char *)buffer, sizeof(buffer) - 1, 0);
                    
                    if (bytes_received <= 0) {
                        std::cout << "Client Disconnected Successfully\n";
                        close(client_fd);
                        pagers.erase(pagers.begin() + i);
                        client_buffers.erase(client_fd);
                    } else {
                        client_buffers[client_fd] = buffer;

                        // --- 4. Week 3 Task: Intercept incoming mutations ---
                        // We pass the raw buffer string elements array directly into our AOF persistence engine
                        // Modify this parsing step to map to your specific custom resp-parser output if needed
                        // e.g., std::vector<std::string> components = parse_resp(client_buffers[client_fd]);
                        //       AOFManager::getInstance().appendCommand(components);

                        std::string response;
                        response = request_router(client_buffers[client_fd].c_str());
                        const char *message = response.c_str();
                        int bytes_sent = send(client_fd, message, strlen(message), 0);
                    }
                }
            }
        }
    }
    
    // Gracefully flush structural strings and close file descriptor lines on exit
    AOFManager::getInstance().stop();
    close(server_fd);
    return 0;
}
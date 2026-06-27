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
#include <algorithm>
// User Header Files
#include "../include/request-router.h"
#include "../include/resp-parser.h"
#include "../include/store.h"  // ← gives us get_store()

// Parses raw RESP buffer into a vector of strings
std::vector<std::string> parse_command(const char* buf) {
    std::vector<std::string> parts;
    std::string s(buf);
    size_t i = 0;
    while (i < s.size()) {
        if (s[i] == '$') {
            size_t end = s.find("\r\n", i);
            if (end == std::string::npos) break;
            int len = std::stoi(s.substr(i + 1, end - i - 1));
            i = end + 2;
            if (i + len > s.size()) break;
            parts.push_back(s.substr(i, len));
            i += len + 2;
        } else {
            i++;
        }
    }
    return parts;
}

int main(int argc, char **argv) {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Failed to create server socket\n";
        return 1;
    }

    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        std::cerr << "setsockopt failed\n";
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(6379);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        std::cerr << "Failed to bind to port 6379\n";
        return 1;
    }

    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0) {
        std::cerr << "listen failed\n";
        return 1;
    }

    std::vector<struct pollfd> pagers;
    pagers.push_back({server_fd, POLLIN, 0});

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    std::cout << "Waiting for a client to connect...\n";

    std::unordered_map<int, std::string> client_buffers;

    while (true) {
        poll(pagers.data(), pagers.size(), -1);

        for (int i = pagers.size() - 1; i >= 0; i--) {
            if (pagers[i].revents & POLLIN) {
                if (pagers[i].fd == server_fd) {
                    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
                    std::cout << "Client connected\n";
                    pagers.push_back({client_fd, POLLIN, 0});
                } else {
                    int client_fd = pagers[i].fd;
                    char buffer[1024];
                    int bytes_received = recv(client_fd, (char *)buffer, sizeof(buffer), 0);

                    if (bytes_received <= 0) {
                        std::cout << "Client Disconnected Successfully\n";
                        close(client_fd);
                        pagers.erase(pagers.begin() + i);
                        client_buffers.erase(client_fd);
                    } else {
                        client_buffers[client_fd] = std::string(buffer, bytes_received);
                        const char* raw = client_buffers[client_fd].c_str();

                        std::vector<std::string> parts = parse_command(raw);
                        std::string response;

                        if (!parts.empty()) {
                            std::string cmd = parts[0];
                            for (char &c : cmd) c = toupper(c);

                            if (cmd == "TYPE" && parts.size() >= 2) {
                                // ── TYPE command ──
                                // Uses the shared get_store() so it works
                                // with ALL commands (SET, RPUSH, ZADD etc.)
                                std::string key = parts[1];
                                auto& store = get_store();
                                auto it = store.find(key);
                                if (it == store.end() || is_expired(it->second)) {
                                    response = "+none\r\n";
                                } else {
                                    // For now all keys in store are strings
                                    // When list/zset added, update Entry to
                                    // have a type field
                                    response = "+string\r\n";
                                }
                            } else {
                                // All other commands → request_router
                                response = request_router(raw);
                            }
                        }

                        const char *message = response.c_str();
                        send(client_fd, message, strlen(message), 0);
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
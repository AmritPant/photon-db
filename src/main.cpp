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
int main(int argc, char **argv) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    // 1) Creating A Sokcet
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Failed to create server socket\n";
        return 1;
    }
    // ensures that we don't run into 'Address already in use' errors
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        std::cerr << "setsockopt failed\n";
        return 1;
    }
    // 2)  Binding a Socket
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(6379);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        std::cerr << "Failed to bind to port 6379\n";
        return 1;
    }
    // 1) Listening to the Socket
    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0) {
        std::cerr << "listen failed\n";
        return 1;
    }
    // Creating a array of Polls
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
                    // Receving the message
                    int client_fd = pagers[i].fd;
                    char buffer[1024];
                    int bytes_received = recv(client_fd, (char *)buffer, sizeof(buffer), 0);
                    if (bytes_received <= 0) {
                        std::cout << "Client Disconnected Sucessfully";
                        close(client_fd);
                        pagers.erase(pagers.begin() + i);
                        // Erasing
                        client_buffers.erase(client_fd);
                    } else {
                        // Sending the message
                        client_buffers[client_fd] = buffer;
                        std::string response;
                        response = request_router(client_buffers[client_fd].c_str());
                        const char *message = response.c_str();
                        int bytes_sent = send(client_fd, message, strlen(message), 0);
                    }
                }
            }
        }
    }
    close(server_fd);
    return 0;
}

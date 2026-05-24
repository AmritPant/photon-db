#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <poll.h> // The magic pager system

int main() {
  // 1. Setup your server_fd and bind/listen exactly as you did before
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  int reuse = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
  
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(6379);
  bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  listen(server_fd, 5);

  // 2. Create the "List of Pagers"
  // pollfd is a struct that holds a socket descriptor and the events we care about
  std::vector<struct pollfd> pagers;
  
  // Give the very first pager to the Maître D' (server_fd)
  // POLLIN means "tell me when there is data INbound" (for server_fd, this means a new connection)
  pagers.push_back({server_fd, POLLIN, 0});

  std::cout << "Waiting for clients...\n";

  // 3. THE EVENT LOOP
  while (true) {
    // Hand the list of pagers to the operating system.
    // The -1 means "block forever until AT LEAST ONE pager buzzes."
    poll(pagers.data(), pagers.size(), -1);

    // If we get past the line above, something buzzed! Let's find out who.
    // We loop through our list backwards (so it's safe to delete disconnected clients)
    for (int i = pagers.size() - 1; i >= 0; i--) {
      
      // Did THIS specific pager buzz with inbound data (POLLIN)?
      if (pagers[i].revents & POLLIN) {

        // Scenario A: The Maître D' buzzed! A new customer is at the door.
        if (pagers[i].fd == server_fd) {
          struct sockaddr_in client_addr;
          int client_len = sizeof(client_addr);
          int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_len);
          
          std::cout << "New client connected!\n";
          
          // Give this new customer their own pager and add them to the list
          pagers.push_back({client_fd, POLLIN, 0});
        } 
        // Scenario B: A specific customer's pager buzzed! They sent us a command.
        else {
          int client_fd = pagers[i].fd;
          char buffer[1024] = {0};
          int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

          if (bytes_received <= 0) {
            // Customer left the restaurant.
            std::cout << "Client disconnected.\n";
            close(client_fd);
            pagers.erase(pagers.begin() + i); // Take back their pager
          } else {
            // Customer sent an order! Since we know they are ready, this won't block.
            std::cout << "Received data!\n";
            std::string response = "+PONG\r\n";
            send(client_fd, response.c_str(), response.length(), 0);
          }
        }
      }
    }
  }

  close(server_fd);
  return 0;
}

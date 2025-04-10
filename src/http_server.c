#include "http_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int http_server_init(http_server_t* server, uint16_t port) {
    server->port = port;
    
    // Create socket
    server->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->server_fd < 0) {
        perror("Failed to create socket");
        return -1;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Failed to set socket options");
        return -1;
    }

    // Configure server address
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind socket
    if (bind(server->server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Failed to bind socket");
        return -1;
    }

    return 0;
}

void http_server_start(http_server_t* server) {
    // Listen for connections
    if (listen(server->server_fd, MAX_PENDING_CONNECTIONS) < 0) {
        perror("Failed to listen");
        return;
    }

    printf("Server listening on port %d...\n", server->port);

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    while (1) {
        int client_fd = accept(server->server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_fd < 0) {
            perror("Failed to accept connection");
            continue;
        }

        printf("New connection from %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), 
               ntohs(client_addr.sin_port));

        handle_client_connection(client_fd);
        close(client_fd);
    }
}

void http_server_stop(http_server_t* server) {
    if (server->server_fd >= 0) {
        close(server->server_fd);
        server->server_fd = -1;
    }
}

void handle_client_connection(int client_fd) {
    char buffer[MAX_REQUEST_SIZE] = {0};
    ssize_t bytes_read = read(client_fd, buffer, MAX_REQUEST_SIZE - 1);
    
    if (bytes_read > 0) {
        printf("Received request:\n%s\n", buffer);

        // Send a simple HTTP response
        const char* response = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/plain\r\n"
                             "Content-Length: 13\r\n"
                             "\r\n"
                             "Hello, World!";

        write(client_fd, response, strlen(response));
    }
}
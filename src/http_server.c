/*
 * Copyright (c) 2025
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "http_server.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

int http_server_init(http_server_t *server, uint16_t port)
{
    // Validate port
    if (port == 0)
    {
        return -1;
    }

    server->port = port;
    server->running = 0;

    // Create socket
    server->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->server_fd < 0)
    {
        return -1;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(server->server_fd);
        server->server_fd = -1;
        return -1;
    }

    // Configure server address
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind socket
    if (bind(server->server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        close(server->server_fd);
        server->server_fd = -1;
        return -1;
    }

    return 0;
}

void http_server_start(http_server_t *server)
{
    // Listen for connections
    if (listen(server->server_fd, MAX_PENDING_CONNECTIONS) < 0)
    {
        perror("Failed to listen");
        return;
    }

    printf("Server listening on port %d...\n", server->port);
    server->running = 1;

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    while (server->running)
    {
        // Set a timeout so we can check running flag
        struct timeval tv = {.tv_sec = 1, .tv_usec = 0};
        setsockopt(server->server_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        int client_fd =
            accept(server->server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd < 0)
        {
            if (server->running)
            {
                perror("Failed to accept connection");
            }
            continue;
        }

        printf("New connection from %s:%d\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        handle_client_connection(client_fd);
        close(client_fd);
    }
}

void http_server_stop(http_server_t *server)
{
    server->running = 0;
    if (server->server_fd >= 0)
    {
        close(server->server_fd);
        server->server_fd = -1;
    }
}

void handle_client_connection(int client_fd)
{
    char buffer[MAX_REQUEST_SIZE] = {0};
    ssize_t bytes_read = read(client_fd, buffer, MAX_REQUEST_SIZE - 1);

    if (bytes_read > 0)
    {
        printf("Received request:\n%s\n", buffer);

        const char *response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/plain\r\n"
                               "Content-Length: 13\r\n"
                               "\r\n"
                               "Hello, World!";

        write(client_fd, response, strlen(response));
    }
}
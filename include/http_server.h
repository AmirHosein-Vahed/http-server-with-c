#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <stdint.h>

#define MAX_REQUEST_SIZE 4096
#define DEFAULT_PORT 8080
#define MAX_PENDING_CONNECTIONS 10

typedef struct {
    int server_fd;
    uint16_t port;
    volatile int running;  // Flag to control server loop
} http_server_t;

// Server lifecycle functions
int http_server_init(http_server_t *server, uint16_t port);
void http_server_start(http_server_t *server);
void http_server_stop(http_server_t *server);

// Request handling
void handle_client_connection(int client_fd);

#endif  // HTTP_SERVER_H
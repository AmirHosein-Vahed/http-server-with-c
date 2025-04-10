#include "http_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static http_server_t server;

void handle_signal(int signum) {
    printf("\nShutting down server...\n");
    http_server_stop(&server);
    exit(0);
}

int main(int argc, char *argv[]) {
    uint16_t port = DEFAULT_PORT;
    
    // Allow port override from command line
    if (argc > 1) {
        port = (uint16_t)atoi(argv[1]);
    }

    // Set up signal handling for graceful shutdown
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    // Initialize and start the server
    if (http_server_init(&server, port) != 0) {
        fprintf(stderr, "Failed to initialize server\n");
        return 1;
    }

    http_server_start(&server);

    return 0;
}
#include "unity.h"
#include "http_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

http_server_t server;
const uint16_t TEST_PORT = 8081;

void setUp(void) {
    // Initialize before each test
    memset(&server, 0, sizeof(http_server_t));
}

void tearDown(void) {
    // Cleanup after each test
    if (server.server_fd >= 0) {
        http_server_stop(&server);
    }
}

void test_server_initialization(void) {
    int result = http_server_init(&server, TEST_PORT);
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_NOT_EQUAL_INT(-1, server.server_fd);
    TEST_ASSERT_EQUAL_INT(TEST_PORT, server.port);
}

void test_server_initialization_invalid_port(void) {
    int result = http_server_init(&server, 0);  // Invalid port
    TEST_ASSERT_EQUAL_INT(-1, result);
}

// Helper function to simulate a client connection
static int connect_to_server(uint16_t port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }
    return sock;
}

// Thread function to run server
static void* run_server(void* arg) {
    http_server_t* server = (http_server_t*)arg;
    http_server_start(server);
    return NULL;
}

void test_server_accept_connection(void) {
    // Initialize and start server in a separate thread
    TEST_ASSERT_EQUAL_INT(0, http_server_init(&server, TEST_PORT));
    
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, run_server, &server);
    
    // Give the server time to start
    usleep(100000);  // 100ms
    
    // Try to connect
    int client_sock = connect_to_server(TEST_PORT);
    TEST_ASSERT_NOT_EQUAL_INT(-1, client_sock);
    
    // Send a simple HTTP request
    const char* request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    ssize_t sent = send(client_sock, request, strlen(request), 0);
    TEST_ASSERT_EQUAL_INT(strlen(request), sent);
    
    // Read response
    char response[1024] = {0};
    ssize_t received = recv(client_sock, response, sizeof(response) - 1, 0);
    TEST_ASSERT_GREATER_THAN_INT(0, received);
    TEST_ASSERT_NOT_NULL(strstr(response, "HTTP/1.1 200 OK"));
    
    // Cleanup
    close(client_sock);
    http_server_stop(&server);
    pthread_join(server_thread, NULL);
}

void test_server_stop(void) {
    TEST_ASSERT_EQUAL_INT(0, http_server_init(&server, TEST_PORT));
    http_server_stop(&server);
    TEST_ASSERT_EQUAL_INT(-1, server.server_fd);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_server_initialization);
    RUN_TEST(test_server_initialization_invalid_port);
    RUN_TEST(test_server_accept_connection);
    RUN_TEST(test_server_stop);
    
    return UNITY_END();
}
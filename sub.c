#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "keyValStore.h"

#define BUFFER_SIZE 256

void handle_command(int client_socket, char* command) {
    char key[100], value[100], response[256];
    if (strncmp(command, "PUT", 3) == 0) {
        sscanf(command + 4, "%s %s", key, value);
        put(key, value);
        snprintf(response, sizeof(response), "PUT:%s:%s\n", key, value);
    } else if (strncmp(command, "GET", 3) == 0) {
        sscanf(command + 4, "%s", key);
        if (get(key, value) > 0)
            snprintf(response, sizeof(response), "GET:%s:%s\n", key, value);
        else
            snprintf(response, sizeof(response), "GET:%s:key_nonexistent\n", key);
    } else if (strncmp(command, "DEL", 3) == 0) {
        sscanf(command + 4, "%s", key);
        if (del(key) > 0)
            snprintf(response, sizeof(response), "DEL:%s:key_deleted\n", key);
        else
            snprintf(response, sizeof(response), "DEL:%s:key_nonexistent\n", key);
    } else if (strncmp(command, "QUIT", 4) == 0) {
        snprintf(response, sizeof(response), "Goodbye!\n");
        send(client_socket, response, strlen(response), 0);
        close(client_socket);
        exit(0);
    } else {
        snprintf(response, sizeof(response), "Unknown command\n");
    }
    send(client_socket, response, strlen(response), 0);
}

void start_server(int port) {
    int server_fd, client_socket;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 1);

    socklen_t addrlen = sizeof(address);
    client_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_socket, buffer, BUFFER_SIZE, 0);
        handle_command(client_socket, buffer);
    }
}

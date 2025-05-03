#include "sub.h"
#include "keyValStore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

void handle_command(int client_socket, char *buffer) {
    char command[5], key[100], value[100], response[BUFFER_SIZE];
    memset(response, 0, BUFFER_SIZE);

    int args = sscanf(buffer, "%s %s %s", command, key, value);

    if (strcmp(command, "PUT") == 0 && args == 3) {
        put(key, value);
        snprintf(response, BUFFER_SIZE, "PUT:%s:%s\n", key, value);
    } else if (strcmp(command, "GET") == 0 && args >= 2) {
        char result[100];
        if (get(key, result) == 0) {
            snprintf(response, BUFFER_SIZE, "GET:%s:%s\n", key, result);
        } else {
            snprintf(response, BUFFER_SIZE, "GET:%s:key_nonexistent\n", key);
        }
    } else if (strcmp(command, "DEL") == 0 && args >= 2) {
        if (del(key) == 0) {
            snprintf(response, BUFFER_SIZE, "DEL:%s:key_deleted\n", key);
        } else {
            snprintf(response, BUFFER_SIZE, "DEL:%s:key_nonexistent\n", key);
        }
    } else if (strcmp(command, "QUIT") == 0) {
        snprintf(response, BUFFER_SIZE, "Connection closed.\n");
        send(client_socket, response, strlen(response), 0);
        close(client_socket);
        exit(0);
    } else {
        snprintf(response, BUFFER_SIZE, "Invalid command.\n");
    }

    send(client_socket, response, strlen(response), 0);
}

void start_server(int port) {
    int server_fd, client_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 1);

    printf("Server gestartet auf Port %d\n", port);
    client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_socket, buffer, BUFFER_SIZE, 0);
        handle_command(client_socket, buffer);
    }
}

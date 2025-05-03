#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "keyValStore.h"
#include "sub.h"

#define BUFFER_SIZE 1024

void handle_command(int client_socket, char* buffer) {
    char* command = strtok(buffer, " \n");
    char* key = strtok(NULL, " \n");
    char* value = strtok(NULL, " \n");
    char response[BUFFER_SIZE];

    if (command == NULL || key == NULL) {
        snprintf(response, sizeof(response), "Invalid command\n");
        send(client_socket, response, strlen(response), 0);
        return;
    }

    if (strcmp(command, "GET") == 0) {
        char result[MAX_VALUE_LEN];
        if (get(key, result) == 0) {
            snprintf(response, sizeof(response), "GET:%s:%s\n", key, result);
        } else {
            snprintf(response, sizeof(response), "GET:%s:key_nonexistent\n", key);
        }
    } else if (strcmp(command, "PUT") == 0) {
        if (value == NULL) {
            snprintf(response, sizeof(response), "PUT:%s:missing_value\n", key);
        } else {
            put(key, value);
            snprintf(response, sizeof(response), "PUT:%s:%s\n", key, value);
        }
    } else if (strcmp(command, "DEL") == 0) {
        if (del(key) == 0) {
            snprintf(response, sizeof(response), "DEL:%s:key_deleted\n", key);
        } else {
            snprintf(response, sizeof(response), "DEL:%s:key_nonexistent\n", key);
        }
    } else if (strcmp(command, "QUIT") == 0) {
        snprintf(response, sizeof(response), "Bye!\n");
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

    printf("Server gestartet auf Port %d\n", port);

    socklen_t addrlen = sizeof(address);
    client_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_socket, buffer, BUFFER_SIZE, 0);
        handle_command(client_socket, buffer);
    }
}

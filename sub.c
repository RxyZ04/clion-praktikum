#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "keyValStore.h"
#include "sub.h"

#define BUFFER_SIZE 1024

void handle_command(int client_socket, char* buffer) {
    char command[BUFFER_SIZE];
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
    char response[BUFFER_SIZE] = {0};

    int count = sscanf(buffer, "%s %s %s", command, key, value);

    if (strcmp(command, "PUT") == 0 && count == 3) {
        put(key, value);
        snprintf(response, sizeof(response), "PUT:%s:%s\n", key, value);
    } else if (strcmp(command, "GET") == 0 && count >= 2) {
        char res[MAX_VALUE_LEN];
        if (get(key, res) >= 0)
            snprintf(response, sizeof(response), "GET:%s:%s\n", key, res);
        else
            snprintf(response, sizeof(response), "GET:%s:key_nonexistent\n", key);
    } else if (strcmp(command, "DEL") == 0 && count >= 2) {
        if (del(key) >= 0)
            snprintf(response, sizeof(response), "DEL:%s:key_deleted\n", key);
        else
            snprintf(response, sizeof(response), "DEL:%s:key_nonexistent\n", key);
    } else if (strcmp(command, "QUIT") == 0) {
        snprintf(response, sizeof(response), "Verbindung beendet\n");
        send(client_socket, response, strlen(response), 0);
        close(client_socket);
        exit(0);
    } else {
        snprintf(response, sizeof(response), "Unbekanntes Kommando\n");
    }

    send(client_socket, response, strlen(response), 0);
}

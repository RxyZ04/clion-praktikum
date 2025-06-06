#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>

#include "keyValStore.h"
#include "sub.h"

#define BUFFER_SIZE 1024

void handle_command(int client_socket, char* buffer) {
    char response[BUFFER_SIZE];

    char* command = strtok(buffer, " \r\n");
    char* key     = strtok(NULL, " \r\n");
    char* value   = strtok(NULL, " \r\n");

    if (client_socket +1) {
        printf("TEST");
    }

    if (!command || !key) {
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
        if (!value) {
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

void handle_sigchld(int sig) {
    // Verwaiste Kindprozesse aufräumen
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void start_server(int port) {
    int server_fd;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];

    // Initialisiere Shared Memory
    if (init_store() != 0) {
        perror("Shared Memory init fehlgeschlagen");
        exit(1);
    }

    signal(SIGCHLD, handle_sigchld); // Behandle beendete Kindprozesse

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket-Erstellung fehlgeschlagen");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind fehlgeschlagen");
        exit(1);
    }

    listen(server_fd, 10);
    printf("Multiclient-Server läuft auf Port %d...\n", port);

    while (1) {
        socklen_t addrlen = sizeof(address);
        int client_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (client_socket < 0) {
            perror("Accept fehlgeschlagen");
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {
            // Kindprozess
            close(server_fd);
            printf("Neuer Client verbunden (PID: %d)\n", getpid());

            const char* welcome =
                "Verfügbare Kommandos:\n"
                "  - PUT <key> <value>\n"
                "  - GET <key>\n"
                "  - DEL <key>\n"
                "  - QUIT\n\n";
            send(client_socket, welcome, strlen(welcome), 0);

            while (1) {
                memset(buffer, 0, BUFFER_SIZE);
                ssize_t bytes = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
                if (bytes <= 0) {
                    printf("[PID %d] Verbindung beendet.\n", getpid());
                    break;
                }

                buffer[bytes] = '\0';
                printf("[PID %d] Empfangen: %s\n", getpid(), buffer);

                handle_command(client_socket, buffer);
            }

            close(client_socket);
            detach_store();
            exit(0);
        } else if (pid > 0) {
            // Elternprozess
            close(client_socket);
        } else {
            perror("Fork fehlgeschlagen");
        }
    }

    destroy_store(); // Nur bei Shutdown
    close(server_fd);
}

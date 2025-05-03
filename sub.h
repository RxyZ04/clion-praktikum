#ifndef SUB_H
#define SUB_H

void start_server(int port);
void handle_command(int client_socket, char* buffer);

#endif

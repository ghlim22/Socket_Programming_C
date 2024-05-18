#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "Practical.h"

#define BUF_SIZE (10)
// #define MAX_PENDING (5)

void handle_client(int sock);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        exit_with_user_message("Parameter(s)", "<Server Port>");
    }

    const char *service = argv[1];

    int serv_sock = setup_tcp_server_socket(service);
    if (serv_sock < 0) {
        exit_with_user_message("socket setup failed", service);
    }

    while (1) {
        int client_sock = accept_tcp_connection(serv_sock);
        handle_client(client_sock);
        close(client_sock);
    }

    return 0;
}

void handle_client(int sock)
{
    char buf[BUF_SIZE];
    ssize_t sent;
    ssize_t received;

    received = recv(sock, buf, BUF_SIZE, 0);
    if (received < 0) {
        exit_with_system_message("recv() failed");
    }

    while (received > 0) {
        sent = send(sock, buf, received, 0);
        if (sent < 0) {
            exit_with_system_message("accept() failed");
        } else if (sent != received) {
            exit_with_user_message("send()", "sent unexpected number of bytes");
        }

        received = recv(sock, buf, BUF_SIZE, 0);
        if (received < 0) {
            exit_with_system_message("recv() failed");
        }
    }
}
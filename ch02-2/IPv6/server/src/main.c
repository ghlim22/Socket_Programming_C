#include "practical.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE (10)
#define MAX_PENDING (5)

void handle_client(int sock);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        exit_with_user_message("Parameter(s)", "<Server Port>");
    }

    in_port_t server_port = atoi(argv[1]);

    /* Create socket for incoming connections */
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_sock < 0) {
        exit_with_system_message("socket() failed");
    }

    /* Construct local address structure */
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(server_port);

    /* Bind to local address */
    if (bind(serv_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
        0) {
        exit_with_system_message("bind() failed");
    }

    /* Mark the socket so it will listen for incoming connections */
    if (listen(serv_sock, MAX_PENDING) < 0) {
        exit_with_system_message("listen() failed");
    }

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        int client_sock = accept(serv_sock, (struct sockaddr *)&client_addr,
                                 &client_addr_len);
        if (client_sock < 0) {
            exit_with_system_message("accept() failed");
        }

        /* client_sock is connected to a client! */

        char client_name[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_name,
                      client_addr_len) != NULL) {
            printf("handling client %s/%d\n", client_name,
                   ntohs(client_addr.sin_port));
        } else {
            puts("Unable to get client address");
        }

        handle_client(client_sock);
    }
    return 0;
}

void handle_client(int sock)
{
    char buf[BUF_SIZE];

    ssize_t received = recv(sock, buf, BUF_SIZE, 0);
    if (received < 0) {
        exit_with_system_message("recv() failed");
    }

    while (received > 0) {
        ssize_t sent = send(sock, buf, received, 0);
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

    close(sock);
}
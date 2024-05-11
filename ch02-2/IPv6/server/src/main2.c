#include <arpa/inet.h>
#include <bits/types/struct_iovec.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "practical.h"

#define BUF_SIZE (10)
#define MAX_PENDING (5)

void print_client_info(struct sockaddr_in6 *addr);
void handle_client(int sock);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        exit_with_user_message("Parameter(s)", "<Server Port>");
    }

    /* Construct the server address structure */
    in_port_t serv_port = atoi(argv[1]);
    struct sockaddr_in6 serv_addr;
    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_port = htons(serv_port);
	serv_addr.sin6_addr = in6addr_any;

    /* Create the listening socket */
    int serv_sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

    /* Bind the listening socket to the server's address and port */
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        exit_with_system_message("bind() failed");
    }

    if (listen(serv_sock, MAX_PENDING) < 0) {
        exit_with_system_message("listen() failed");
    }

    while (1) {
        struct sockaddr_in6 client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_sock = accept(serv_sock, (struct sockaddr *)&client_addr,
                                 &client_addr_len);
        if (client_sock < 0) {
            exit_with_system_message("accept() failed");
        }
		print_client_info(&client_addr);
		handle_client(client_sock);
    }

    return 0;
}

void print_client_info(struct sockaddr_in6 *addr)
{
    char buf[INET6_ADDRSTRLEN];
    in_port_t client_port = ntohs(addr->sin6_port);
    if (inet_ntop(AF_INET6, (struct sockaddr *)addr, buf, sizeof(buf)) !=
        NULL) {
        printf("handling client %s/%d\n", buf, client_port);
    } else {
        printf("Can't get client address\n");
    }
}

void handle_client(int sock)
{
    char buf[BUF_SIZE];
    ssize_t received = recv(sock, buf, BUF_SIZE - 1, 0);
    if (received < 0) {
        exit_with_system_message("recv() failed");
    }

    while (received > 0) {
        ssize_t sent = send(sock, buf, received, 0);
        if (sent < 0) {
            exit_with_system_message("send() failed");
        } else if (sent != received) {
            exit_with_user_message("send()", "sent unexpected number of bytes");
        }

        received = recv(sock, buf, BUF_SIZE - 1, 0);
        if (received < 0) {
            exit_with_system_message("recv() failed");
        }
    }

    close(sock);
}

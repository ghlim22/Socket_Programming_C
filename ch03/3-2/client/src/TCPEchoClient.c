#include <arpa/inet.h>
#include <bits/types/struct_iovec.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "Practical.h"

#define BUF_SIZE (100)
#define DEFAULT_ECHO_PORT (7)

void receive_greeting(int sock);
void print_connection_info(int sock);
void exercise7(int sock);
void exercise9(int sock);

int main(int argc, char *argv[])
{
    if (argc < 3 || argc > 4) {
        exit_with_user_message("Parameter(s)",
                               "<Server Address> <Echo Word> [<Server Port>]");
    }

    const char *server = argv[1];
    const char *echo_string = argv[2];
    const char *service = (argc == 4) ? argv[3] : "echo";
    // in_port_t server_port = (argc == 4) ? atoi(argv[3]) : DEFAULT_ECHO_PORT;

    /* Create a socket and connect to the server */
    int sock = setup_tcp_client_socket(server, service);
    if (sock < 0) {
        exit_with_user_message("socket setup failed", "unable to connect");
    }

    /* Print peer and local address information */
    print_connection_info(sock);

    /* Send the string to the server */
    size_t echo_strlen = strlen(echo_string);
    ssize_t num_bytes = send(sock, echo_string, echo_strlen, 0);
    if (num_bytes < 0) {
        exit_with_system_message("send() failed");
    } else if ((size_t)num_bytes != echo_strlen) {
        exit_with_user_message("send()", "sent unexpected number of bytes");
    }

    /* Receive the same string back from the server */
    unsigned int total_bytes_rcvd = 0;
    fputs("Received: ", stdout);
    while (total_bytes_rcvd < echo_strlen) {
        char buf[BUF_SIZE];
        num_bytes = recv(sock, buf, BUF_SIZE - 1, 0);
        if (num_bytes < 0) {
            exit_with_system_message("recv() failed");
        } else if (num_bytes == 0) {
            exit_with_user_message("recv()", "connection closed prematurely");
        }
        total_bytes_rcvd += num_bytes;
        buf[num_bytes] = '\0';
        fputs(buf, stdout);
    }
    fputc('\n', stdout);
    close(sock);

    INET_ADDRSTRLEN;

    return 0;
}

void print_connection_info(int sock)
{
    struct sockaddr_in peer;
    struct sockaddr_in local;
    socklen_t addr_len;
    int ret_val;
    char buf[INET_ADDRSTRLEN];

    addr_len = sizeof(peer);
    ret_val = getpeername(sock, (struct sockaddr *)&peer, &addr_len);
    if (ret_val < 0) {
        exit_with_system_message("getpeername() failed");
    }
    if (inet_ntop(AF_INET, (struct in_addr *)&peer, buf, sizeof(buf)) != NULL) {
        printf("peer: %s/%d\n", buf, ntohs(peer.sin_port));
    }

    addr_len = sizeof(local);
    ret_val = getsockname(sock, (struct sockaddr *)&local, &addr_len);
    if (ret_val < 0) {
        exit_with_system_message("getsockname() failed");
    }
    if (inet_ntop(AF_INET, (struct in_addr *)&local, buf, sizeof(buf)) !=
        NULL) {
        printf("local: %s/%d\n", buf, ntohs(local.sin_port));
    }
}

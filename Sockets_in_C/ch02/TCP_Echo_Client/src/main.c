#include "Practical.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE (100)

/* A TCP IPv4 echo client */
int main(int ac, char *av[])
{
    /* 1. Application setup and parameter parsing */
    if (ac < 3 || ac > 4) {
        die_with_user_message("Parameter(s)",
                              "<Server Address> <Echo String> [<Server Port>]");
    }

    const char *serv_ip = av[1];
    const char *echo_str = av[2];
    in_port_t serv_port =
        (ac == 4) ? atoi(av[3]) : 7; /* 7 is the well-known echo port */

    /* 2. TCP socket creation */

    /* Create a reliable, stream socket using TCP */
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        die_with_system_message("socket() failed");
    }

    /* 3. Prepare address and establish connection */

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; /* IPv4 address family */
    int ret_val = inet_pton(AF_INET, serv_ip, &serv_addr.sin_addr.s_addr);
    if (ret_val == 0) {
        die_with_user_message("inet_pton() failed", "invalid address string");
    } else if (ret_val < 0) {
        die_with_system_message("inet_pton() failed");
    }
    serv_addr.sin_port = htons(serv_port);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        die_with_system_message("connect() failed");
    }

    /* 4. Send echo string to server */

    size_t echo_str_len = strlen(echo_str);
    ssize_t num_bytes = send(sock, echo_str, echo_str_len, 0);
    if (num_bytes < 0) {
        die_with_system_message("send() failed");
    } else if (num_bytes != (ssize_t)echo_str_len) {
        die_with_user_message("send()", "sent unexpected number of bytes");
    }

    /* 5. Receive echo server reply */

    unsigned int total_bytes_rcvd = 0;
    fputs("Received: ", stdout);
    while (total_bytes_rcvd < echo_str_len) {
        char buffer[BUF_SIZE];
        /* Receive up to the buffer size (minus 1 to leave space for a null
           terminator) bytes from the sender */
        num_bytes = recv(sock, buffer, BUF_SIZE - 1, 0);
        if (num_bytes < 0) {
            die_with_system_message("recv() failed");
        } else if (num_bytes == 0) {
            die_with_user_message("recv()", "connection closed prematurely");
        }
        total_bytes_rcvd += num_bytes; /* Keep tally of total bytes */
        buffer[num_bytes] = '\0';      /* Terminate the string */
        fputs(buffer, stdout);
    }

    fputc('\n', stdout);

    close(sock);

    return 0;
}

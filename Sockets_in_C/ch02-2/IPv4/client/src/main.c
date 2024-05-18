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
#include "practical.h"

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

    const char *server_ip = argv[1];
    const char *echo_string = argv[2];
    in_port_t server_port = (argc == 4) ? atoi(argv[3]) : DEFAULT_ECHO_PORT;

    /* Create a reliable, stream socket using TCP */
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        exit_with_system_message("socket() failed");
    }

    /* Construct the server address structure */
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    int rtn_val = inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
    if (rtn_val == 0) {
        exit_with_user_message("inet_pton() failed", "invalid address string");
    } else if (rtn_val < 0) {
        exit_with_system_message("inet_pton() failed");
    }
    server_addr.sin_port = htons(server_port);

    // exercise7(sock);
    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
        0) {
        exit_with_system_message("connect() failed");
    }

    /* exercise 2-9. bind the port after connect() */
    /* bind() fails - invalid fd */
    exercise9(sock);

    /* Print peer and local address information */
    print_connection_info(sock);

    /* Receive greeting message from the server */
    // receive_greeting(sock);

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

void receive_greeting(int sock)
{
    ssize_t total = 0;
    ssize_t received = 0;
    ssize_t buf_size = 100;
    char buf[buf_size];

    while (total < buf_size - 1) {
        received = recv(sock, buf + total, buf_size - total - 1, 0);
        if (received < 0) {
            exit_with_system_message("recv() failed");
        } else if (received == 0) {
            exit_with_user_message("recv()", "connection closed prematurely");
        }
        total += received;
    }
    buf[buf_size - 1] = '\0';

    if (strncmp(GREETING_MSG, buf, strlen(GREETING_MSG)) == 0 &&
        strlen(GREETING_MSG) == strlen(buf)) {
        printf("message from server: ");
        puts(buf);
        putc('\n', stdout);
    } else {
        printf("message from server: ");
        puts(buf);
        putc('\n', stdout);
        exit_with_user_message("greetings", "wrong welcome message");
    }
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

void exercise7(int sock)
{
    char buf[INET_ADDRSTRLEN];
    struct sockaddr_in peer;
    struct sockaddr_in local;
    socklen_t addr_len;

    addr_len = sizeof(local);
    if (getsockname(sock, (struct sockaddr *)&local, &addr_len) < 0) {
        exit_with_system_message("2-7. getsockname() failed");
    }
    if (inet_ntop(AF_INET, (struct in_addr *)&local, buf, sizeof(buf)) !=
        NULL) {
        printf("2-7. local: %s/%d\n", buf, ntohs(local.sin_port));
    }

    addr_len = sizeof(peer);
    if (getpeername(sock, (struct sockaddr *)&peer, &addr_len) < 0) {
        exit_with_system_message("2-7. getpeername() failed");
    }
    if (inet_ntop(AF_INET, (struct in_addr *)&peer, buf, sizeof(buf)) != NULL) {
        printf("2-7. peer: %s/%d\n", buf, ntohs(peer.sin_port));
    }
}

void exercise9(int sock)
{
    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(35432);
    if (bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0) {
        exit_with_system_message("bind() failed");
    }
}

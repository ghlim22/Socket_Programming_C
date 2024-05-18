#include "practical.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

void exit_with_user_msg(const char *msg, const char *detail)
{
    fputs(msg, stderr);
    fputs(": ", stderr);
    fputs(detail, stderr);
    fputs("\n", stderr);
    exit(EXIT_FAILURE);
}

void exit_with_system_msg(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void print_client_info(const struct sockaddr_in6 *addr)
{
    char clnt_name[INET_ADDRSTRLEN];
    in_port_t clnt_port = ntohs(addr->sin6_port);

    if (clnt_port < 0 || inet_ntop(AF_INET, &addr->sin6_addr, clnt_name,
                                   INET_ADDRSTRLEN) == NULL) {
        printf("Unable to get client's address\n");
    } else {
        printf("Handling client: %s/%d\n", clnt_name, clnt_port);
    }
}

void receive_greetings(int sock)
{
    char buf[BUF_SIZE];
    ssize_t total_bytes_rcvd = 0;
    const size_t msg_len = strlen(GREETING_MSG);

    while (total_bytes_rcvd < (ssize_t)msg_len) {
        ssize_t rcvd_bytes = recv(sock, buf + total_bytes_rcvd,
                                  BUF_SIZE - total_bytes_rcvd - 1, 0);
        if (rcvd_bytes < 0) {
            exit_with_system_msg("recv() failed");
        }
        total_bytes_rcvd += rcvd_bytes;
        buf[total_bytes_rcvd] = '\0';
    }
    if (strncmp(GREETING_MSG, buf, total_bytes_rcvd) != 0) {
        exit_with_user_msg("invalid greeting msg", buf);
    }
    puts(buf);
}

void get_remote_addr(int sock)
{
    /* Get remote address */
    struct sockaddr_in6 remote_addr;
    char buf[INET6_ADDRSTRLEN];

    socklen_t remote_addr_size = sizeof(remote_addr);
    memset(&remote_addr, 0, remote_addr_size);

    printf("remote_addr_size: %d\n", remote_addr_size);

    if (getpeername(sock, (struct sockaddr *)&remote_addr, &remote_addr_size) <
        0) {
        exit_with_system_msg("getpeername() failed");
    }
    printf("actual remote_addr_size: %d\n", remote_addr_size);
    if (inet_ntop(AF_INET6, &remote_addr.sin6_addr , buf, INET6_ADDRSTRLEN) != NULL) {
        printf("Remote address: %s/%d\n", buf, ntohs(remote_addr.sin6_port));
    } else {
        printf("Unable to get remote address\n");
    }

    for (int i = 0; i < 16; ++i) {
        printf("%02d", remote_addr.sin6_addr.__in6_u.__u6_addr8[i]);
        if ((i + 1) % 2 == 0) {
            printf(":");
        }
    }
    printf("\n");
}

void get_local_addr(int sock)
{
    struct sockaddr_in6 local_addr;
    char buf[INET6_ADDRSTRLEN];

    socklen_t local_addr_size = sizeof(local_addr);
    memset(&local_addr, 0, local_addr_size);

    printf("local_addr_size: %d\n", local_addr_size);
    if (getsockname(sock, (struct sockaddr *)&local_addr, &local_addr_size) < 0) {
        exit_with_system_msg("getsockname() failed");
    }
    printf("actual local_addr_size: %d\n", local_addr_size);
    if (inet_ntop(AF_INET6, &local_addr.sin6_addr , buf, INET6_ADDRSTRLEN) != NULL) {
        printf("Local address: %s/%d\n", buf, ntohs(local_addr.sin6_port));
    } else {
        printf("Unable to get local address\n");
    }

    for (int i = 0; i < 16; ++i) {
        printf("%02d", local_addr.sin6_addr.__in6_u.__u6_addr8[i]);
        if ((i + 1) % 2 == 0) {
            printf(":");
        }
    }
    printf("\n");
}
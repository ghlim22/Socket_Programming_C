#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "practical.h"

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
    char clnt_name[INET6_ADDRSTRLEN];

    if (inet_ntop(AF_INET, &addr->sin6_addr, clnt_name, INET_ADDRSTRLEN) == NULL) {
        printf("Unable to get client's address\n");
    } else {
        printf("Handling client: %s/%d\n", clnt_name, ntohs(addr->sin6_port));
    }
}

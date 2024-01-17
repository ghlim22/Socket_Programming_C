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

void print_client_info(const struct sockaddr_in *addr)
{ 
}

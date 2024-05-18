#include "Practical.h"
#include <stdio.h>
#include <sys/socket.h>

int accept_connection(int serv_sock)
{
    struct sockaddr_storage clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    int sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_len);
    if (sock < 0) {
        exit_with_system_message("accept() failed");
    }
    fputs("handling: ", stdout);
    print_socket_address((struct sockaddr *)&clnt_addr, stdout);
    fputc('\n', stdout);

    return sock;
}
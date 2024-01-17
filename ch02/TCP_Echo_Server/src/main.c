#include "Practical.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const int BUF_SIZE = 100;
static const int MAX_PENDING = 5; /* Maximum outstanding connection requests */

/* An IPv4 TCP echo server */
int main(int ac, char *av[])
{
    /* 1. Application setup and parameter parsing */

    if (ac != 2) {
        die_with_user_message("Parameter(s)", "<Server Port>");
    }

    in_port_t serv_port = atoi(av[1]);

    /* 2. Socket creation and setup */

    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_sock < 0) {
        die_with_system_message("socket() failed");
    }

    /* Construct local address structure */
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));      /* Zero out structure */
    serv_addr.sin_family = AF_INET;                /* IPv4 address family */
    serv_addr.sin_port = htons(serv_port);         /* Local port */
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */

    /* Bind the socket to the local address */
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        die_with_system_message("bind() failed");
    }

    char serv_name[BUF_SIZE];
    inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, serv_name, BUF_SIZE);
    printf("server:%s/%d\n", serv_name, ntohs(serv_addr.sin_port));

    /* Mark the socket so it will listen for incoming connections */
    if (listen(serv_sock, MAX_PENDING) < 0) {
        die_with_system_message("listen() failed");
    }

    /* 3. Iteratively handle incoming connections */

    while (true) {
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_len = sizeof(clnt_addr);

        /* Wait for a client to connect */
        int clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_len);
        if (clnt_sock < 0) {
            die_with_system_message("accept() failed");
        }

        /* clnt_sock is connected to a client! */

        char clnt_name[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &clnt_addr.sin_addr.s_addr, clnt_name, sizeof(clnt_name)) != NULL) {
            printf("Handling client %s/%d\n", clnt_name, ntohs(clnt_addr.sin_port));
        } else {
            puts("Unable to get client address");
        }

        handle_tcp_client(clnt_sock);
    }

    /* Not reached */

    return 0;
}

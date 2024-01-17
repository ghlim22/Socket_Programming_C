#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "practical.h"

int main(int argc, char *argv[])
{
    /* 1. Application setup and parameter parsing */
    if (argc != 2) {
        exit_with_user_msg("Parameter(s)", "<Server Port>");
    }

    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_sock < 0) {
        exit_with_system_msg("socket() failed");
    }
    in_port_t serv_port = atoi(argv[1]);

    /* 2. TCP socket creation and binding */

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any local address */
    serv_addr.sin_port = htons(serv_port);

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        exit_with_system_msg("bind() failed");
    }

    if (listen(serv_sock, MAX_PENDING) < 0) {
        exit_with_system_msg("listen() failed");
    }

    /* 3. Iteratively handle incoming connections */
    
    while (true) {
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_len = sizeof(clnt_addr);
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_len);
        if (clnt_sock < 0) {
            exit_with_system_msg("accept() failed");
        }
        
        echo(clnt_sock);
        close(clnt_sock);
    }

    return (0);
}

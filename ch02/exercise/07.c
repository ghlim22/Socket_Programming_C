#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/*
07. What happens when you call getpeername() on an unconnected TCP socket?
*/

int main(int argc, char *argv[])
{
    struct sockaddr_in6 addr;
    socklen_t addr_len = sizeof(addr);
    int sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

    memset(&addr, 0, sizeof(addr));
    if (getpeername(sock, (struct sockaddr *)&addr, &addr_len) < 0) {
        perror("getpeername() failed");
        // exit(0);
    }
    if (getsockname(sock, (struct sockaddr *)&addr, &addr_len) < 0) {
        perror("getsockname() failed");
        exit(0);
    } else {
        char buf[INET6_ADDRSTRLEN];
        if (inet_ntop(AF_INET6, &addr, buf, INET6_ADDRSTRLEN) != NULL) {
            printf("local address: %s/%d\n", buf, htons(addr.sin6_port));
        }
    }
    return 0;
}
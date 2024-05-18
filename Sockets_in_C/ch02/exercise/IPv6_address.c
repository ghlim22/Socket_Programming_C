
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_port(in_port_t port)
{
    unsigned char *tmp = (unsigned char *)&port;

    for (int i = 0; i < sizeof(port); ++i) {
        printf("[%d]: %02x ", i, tmp[i]);
    }
    printf("\n");
    printf("network order: %04x\n", ntohs(port));
    printf("host order: %04x\n",port);
}

void print_addr(struct in6_addr addr) {
    for (int i = 0; i < 4; ++i) {
        printf("[%d]: %08x ", i, addr.__in6_u.__u6_addr32[i]);
    }
    printf("\n");
    for (int i = 0; i < 8; ++i) {
        printf("[%d]: %04x ", i, addr.__in6_u.__u6_addr16[i]);
    }
    printf("\n");
    for (int i = 0; i < 16; ++i) {
        printf("[%d]: %02x ", i, addr.__in6_u.__u6_addr8[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    struct sockaddr_in6 addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    int ret_val = inet_pton(AF_INET6, argv[1], &addr.sin6_addr);
    if (ret_val == 0) {
        printf("invalid address\n");
        exit(0);
    } else if (ret_val < 0) {
        printf("inet_pton() failed\n");
        exit(0);
    }
    addr.sin6_port = htons(atoi(argv[2]));
    print_port(addr.sin6_port);
    print_addr(addr.sin6_addr);

    return 0;
}

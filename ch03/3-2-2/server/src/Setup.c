#include "Practical.h"
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static const int MAX_PENDING = 5;

static void setup_hints(struct addrinfo *hint)
{
    memset(hint, 0, sizeof(struct addrinfo));
    hint->ai_family = AF_UNSPEC;
    hint->ai_socktype = SOCK_STREAM;
    hint->ai_protocol = IPPROTO_TCP;
    hint->ai_flags = AI_PASSIVE;
}

int setup_server_socket(const char *service)
{
    struct addrinfo hints;
    setup_hints(&hints);

    struct addrinfo *list = get_address_list_or_null(NULL, service, &hints);
    if (list == NULL) {
        return -1;
    }

    int sock = -1;
    for (struct addrinfo *i = list; i != NULL; i = i->ai_next) {
        sock = socket(i->ai_family, i->ai_socktype, i->ai_protocol);
        if (sock < 0) {
            continue;
        }
        if (bind(sock, (struct sockaddr *)i->ai_addr, i->ai_addrlen) == 0 &&
            listen(sock, MAX_PENDING) == 0) {
            fputs("binding to ", stdout);
            print_socket_address((struct sockaddr *)i->ai_addr, stdout);
            fputc('\n', stdout);
            break;
        }
        close(sock);
        sock = -1;
    }

    freeaddrinfo(list);

    return sock;
}

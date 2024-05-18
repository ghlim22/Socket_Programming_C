#include "Practical.h"
#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>

struct addrinfo *get_address_list_or_null(const char *node_or_null,
                                          const char *service,
                                          const struct addrinfo *hints_or_null)
{
    assert(service != NULL);
    struct addrinfo *list = NULL;
    int ecode = getaddrinfo(node_or_null, service, hints_or_null, &list);
    if (ecode != 0) {
        exit_with_user_message("getaddrinfo() failed", gai_strerror(ecode));
    }

    return list;
}

void print_socket_address(const struct sockaddr *address, FILE *stream)
{
    char buf[INET6_ADDRSTRLEN];
    void *numeric_address;
    in_port_t port;

    assert(address != NULL);
    assert(stream != NULL);

    switch (address->sa_family) {
    case AF_INET:
        numeric_address = &((struct sockaddr_in *)address)->sin_addr;
        port = ((struct sockaddr_in *)address)->sin_port;
        break;
    case AF_INET6:
        numeric_address = &((struct sockaddr_in6 *)address)->sin6_addr;
        port = ((struct sockaddr_in6 *)address)->sin6_port;
        break;
    default:
        fputs("[unknown type]", stream);
        return;
    }

    if (inet_ntop(address->sa_family, numeric_address, buf, sizeof(buf)) ==
        NULL) {
        fputs("[invalid address]", stream);
    } else {
        fprintf(stream, "%s", buf);
        if (port != 0) {
            fprintf(stream, "-%u", port);
        }
    }
}

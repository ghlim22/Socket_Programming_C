#include <arpa/inet.h>
#include <assert.h>
#include <stddef.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/_types/_in_port_t.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Practical.h"

struct addrinfo *GetAddrList(const char *host, const char *service,
                             struct addrinfo *hint)
{
    struct addrinfo *addr_list;
    int ret;

    ret = getaddrinfo(host, service, hint, &addr_list);
    if (ret != 0) {
        ExitWithUserMsg("getaddrinfo() failed", gai_strerror(ret));
        // return NULL;
    }

    return addr_list;
}

void PrintSocketAddress(struct sockaddr *addr, FILE *stream)
{
    assert(addr != NULL && stream != NULL);

    void *numericAddress;
    in_port_t port;

    switch (addr->sa_family) {
    case AF_INET:
        numericAddress = &((struct sockaddr_in *)addr)->sin_addr;
        port = ((struct sockaddr_in *)addr)->sin_port;
        break;
    case AF_INET6:
        numericAddress = &((struct sockaddr_in6 *)addr)->sin6_addr;
        port = ((struct sockaddr_in6 *)addr)->sin6_port;
        break;
    default:
        fputs("[unknown type]", stream);
        return;
    }

    char buf[BUF_SIZE];
    if (inet_ntop(addr->sa_family, numericAddress, buf, sizeof(buf)) == NULL) {
        fputs("[invalid address]", stream);
    } else {
        fprintf(stream, "%s", buf);
        if (port != 0) {
            fprintf(stream, "-%u", port);
        }
    }
}

int SetupClient(const char *host, const char *service)
{
    struct addrinfo hint;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = IPPROTO_TCP;

    struct addrinfo *serv_addr_list = GetAddrList(host, service, &hint);
    if (serv_addr_list == NULL) {
        ;
    }

    int sock = -1;
    for (struct addrinfo *addr = serv_addr_list; addr != NULL;
         addr = addr->ai_next) {
        sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sock < 0) {
            continue; /* socket creation failed; try next address */
        }

        if (connect(sock, addr->ai_addr, addr->ai_addrlen) == 0) {
            break;
        }

        close(sock); /* connection failed; try next address */
    }

    freeaddrinfo(serv_addr_list);

    return sock;
}

void SetupHints(struct addrinfo *hints)
{
    memset(hints, 0, sizeof(struct addrinfo));
    hints->ai_family = AF_UNSPEC;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_protocol = IPPROTO_TCP;
}

int SetupServer(const char *service)
{
    struct addrinfo hints;
    SetupHints(&hints);

    struct addrinfo *list = GetAddrList(NULL, service, &hints);
    assert(list != NULL);

    int sock = -1;
    for (struct addrinfo *i = list; i != NULL; i = i->ai_next) {
        sock = socket(i->ai_family, i->ai_socktype, i->ai_protocol);
        if (sock < 0) {
            continue;
        }
        if (bind(sock, (struct sockaddr *)i->ai_addr, i->ai_addrlen) == 0 &&
            listen(sock, MAX_PENDING) == 0) {
            break;
        }
        close(sock);
        sock = -1;
    }
    freeaddrinfo(list);

    return sock;
}

int AcceptConnection(const int listeningSock)
{
    struct sockaddr_storage clientInfo;
    socklen_t clientInfoLen = sizeof(clientInfo);

    int sock =
        accept(listeningSock, (struct sockaddr *)&clientInfo, &clientInfoLen);
    if (sock < 0) {
        ExitWithSystemMsg("accept() failed");
    }
    fputs("handling: ", stdout);
    PrintSocketAddress((struct sockaddr *)&clientInfo, stdout);
    fputc('\n', stdout);
}

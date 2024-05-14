#include <netdb.h>
#include <sys/socket.h>
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
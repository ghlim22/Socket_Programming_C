#include <netdb.h>
#include <sys/socket.h>
#include "practical.h"

struct addrinfo *get_address_info(const char *name, const char *service,
                                  struct addrinfo *hint)
{
    struct addrinfo *addr_list;
    int ret;

    ret = getaddrinfo(name, service, hint, &addr_list);
    if (ret != 0) {
        exit_with_user_message("getaddrinfo() failed", gai_strerror(ret));
        // return NULL;
    }

    return addr_list;
}
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include "practical.h"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        exit_with_user_message("Parameter(s)", "<Address/Name> <Port/Service>");
    }

    char *addr_string = argv[1];
    char *port_string = argv[2];

    /* Tell the system what kind(s) of address info we want */
    struct addrinfo addr_criteria;
    memset(&addr_criteria, 0, sizeof(addr_criteria));
    addr_criteria.ai_family = AF_UNSPEC;
    addr_criteria.ai_socktype = SOCK_STREAM;
    addr_criteria.ai_protocol = IPPROTO_TCP;

    /* Get address(es) associated with the specified name/service */
    struct addrinfo *addr_list =
        get_address_info(addr_string, port_string, &addr_criteria);

	for (struct addrinfo *i = addr_list; i != NULL; i = i->ai_next) {
		print_socket_address(i->ai_addr, stdout);
		fputc('\n', stdout);
	}

	freeaddrinfo(addr_list);

    return 0;
}

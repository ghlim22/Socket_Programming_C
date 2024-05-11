#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Practical.h"

int setup_tcp_client_socket(const char *host, const char *service)
{
	struct addrinfo hint;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_protocol = IPPROTO_TCP;

	struct addrinfo *serv_addr_list = get_addr_list(host, service, &hint);
	if (serv_addr_list == NULL) {
		;
	}

	int sock = -1;
	for (struct addrinfo *addr = serv_addr_list; addr != NULL; addr = addr->ai_next) {
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
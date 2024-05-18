#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

void print_socket_address(const struct sockaddr *addr, FILE *stream)
{
    char buf[INET6_ADDRSTRLEN];
    void *numeric_addr;
    in_port_t port;

    if (addr == NULL || stream == NULL) {
        return;
    }

    switch (addr->sa_family) {
    case AF_INET:
		numeric_addr = &((struct sockaddr_in *)addr)->sin_addr;
		port = ntohs(((struct sockaddr_in *)addr)->sin_port);
        break;
	case AF_INET6:
		numeric_addr = &((struct sockaddr_in6 *)addr)->sin6_addr;
		port = ntohs(((struct sockaddr_in6 *)addr)->sin6_port);
        break;
	default:
	 	fputs("[unknown type]", stream); 
		return;
    }

	if (inet_ntop(addr->sa_family, numeric_addr, buf, sizeof(buf)) == NULL) {
		fputs("[invalid address]", stream);
	} else {
		fprintf(stream, "%s", buf);
		if (port != 0) {
			fprintf(stream, "-%u", port);
		}
	}
}
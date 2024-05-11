#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char hostname_buf[NI_MAXHOST];
    char service_buf[NI_MAXSERV];

    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET;
	addr.sin6_port = htons(atoi(argv[2]));
    int ret = inet_pton(AF_INET6, argv[1], &addr.sin6_addr.s6_addr);
    if (ret <= 0) {
        return 0;
    }

    char buf[INET6_ADDRSTRLEN];
    if (inet_ntop(AF_INET6, &addr.sin6_addr.s6_addr, buf, sizeof(buf)) != NULL) {
        printf("%s/%d\n", buf, ntohs(addr.sin6_port));
    } else {
        ;
    }


    int ecode =
        getnameinfo((struct sockaddr *)&addr, sizeof(addr), hostname_buf,
                    sizeof(hostname_buf), service_buf, sizeof(service_buf), NI_NUMERICHOST);

	if (ecode != 0) {
		return 0;
	}

	printf("host: %s / service: %s\n", hostname_buf, service_buf);

    return 0;
}
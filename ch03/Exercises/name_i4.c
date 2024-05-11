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

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &addr.sin_addr.s_addr);

    int ecode =
        getnameinfo((struct sockaddr *)&addr, sizeof(addr), hostname_buf,
                    sizeof(hostname_buf), service_buf, sizeof(service_buf), 0);

	if (ecode != 0) {
		return 0;
	}

	printf("host: %s / service: %s\n", hostname_buf, service_buf);

    return 0;
}
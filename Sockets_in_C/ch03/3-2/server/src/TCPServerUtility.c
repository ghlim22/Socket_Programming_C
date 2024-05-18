#include "Practical.h"
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static const int MAX_PENDING = 5;

void print_local_addr(int sock)
{
    struct sockaddr_storage local_addr;
    socklen_t addr_len = sizeof(local_addr);
    if (getsockname(sock, (struct sockaddr *)&local_addr, &addr_len) < 0) {
        exit_with_system_message("getsockname() failed");
    }
    fputs("Binding to ", stdout);
    print_socket_address((struct sockaddr *)&local_addr, stdout);
    fputc('\n', stdout);
}

int setup_tcp_server_socket(const char *service)
{
    struct addrinfo hint;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_flags = AI_PASSIVE;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = IPPROTO_TCP;

    struct addrinfo *addr_list = get_addr_list(NULL, service, &hint);
    if (addr_list == NULL) {
        return -1;
    }

	for (struct addrinfo *i = addr_list; i != NULL; i = i->ai_next) {
		print_socket_address(i->ai_addr, stdout);
		fputc('\n', stdout);
	}

    int sock = -1;
    for (struct addrinfo *i = addr_list; i != NULL; i = i->ai_next) {
        sock = socket(i->ai_family, i->ai_socktype, i->ai_protocol);
        if (sock < 0) {
            continue;
        }

        if (bind(sock, i->ai_addr, i->ai_addrlen) == 0 &&
            listen(sock, MAX_PENDING) == 0) {
            print_local_addr(sock);
            break;
        }

        close(sock);
        sock = -1;
    }

	freeaddrinfo(addr_list);

    return sock;
}

int accept_tcp_connection(int serv_sock)
{
	struct sockaddr_storage client_addr;
    socklen_t client_addrlen = sizeof(client_addr);

    int clnt_sock = accept(serv_sock, (struct sockaddr *)&client_addr, &client_addrlen);
    if (clnt_sock < 0) {
        exit_with_system_message("accept() failed");
    }

    fputs("handling client: ", stdout);
    print_socket_address((struct sockaddr *)&client_addr, stdout);
    fputc('\n', stdout);

    return clnt_sock;
}
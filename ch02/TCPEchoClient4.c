#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

void die_with_user_message(const char* msg, const char* detail);
void die_with_system_message(const char* msg);

/* A TCP IPv4 echo client */
int main(int ac, char* av[])
{
	/* 1. Application setup and parameter parsing */
	if (ac < 3 || ac > 4) {
		die_with_user_message("Parameter(s)", "<Server Address> <Echo String> [<Server Port>]");
	}

	const char* serv_ip = av[1];
	const char* echo_str = av[2];
	in_port_t serv_port = (av == 4) ? atoi(av[3]) : 7; /* 7 is the well-known echo port */

	/* 2. TCP socket creation */
	/* Create a reliable, stream socket using TCP */
	int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serv_sock < 0) {
		die_with_system_message("socket() failed");
	}

	/* 3. Prepare address and establish connection */
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; /* IPv4 address family */











	return 0;
}

void die_with_user_message(const char* msg, const char* detail)
{
	fputs(msg, stderr);
	fputs(": ", stderr);
	fputs(detail, stderr);
	fputs("\n", stderr);
	exit(EXIT_FAILURE);
}

void die_with_system_message(const char* msg)
{
	perror(msg);
	exit(EXIT_SUCCESS);
}

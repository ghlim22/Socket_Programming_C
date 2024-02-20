#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "practical.h"

int main(int argc, char *argv[])
{
	/* 1. Application setup and parameter parsing */
	if (!(argc == 3 || argc == 4)) {
		exit_with_user_msg("Parameter(s)", "<Server Address> <Echo String> [<Server Port>]");
	}
	const char *serv_addr_str = argv[1];
	const char *echo_str = argv[2];

	/* 2. Socket creation and address setup */

	in_port_t serv_port = (argc == 4) ? atoi(argv[3]) : 7;
	int sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		exit_with_system_msg("socket() failed");
	}
	
	struct sockaddr_in6 serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr)); /* Zero out the structure */
	serv_addr.sin6_family = AF_INET6;
	serv_addr.sin6_port = htons(serv_port);
	int ret_val = inet_pton(AF_INET6, serv_addr_str, &serv_addr.sin6_addr);
	if (ret_val == 0) {
		exit_with_user_msg("inet_pton()", "invalid server address string");
	} else if (ret_val < 0) {
		exit_with_system_msg("inet_pton() failed");
	}

	/* ex02 - 8 */
	// struct sockaddr_in6 clnt_addr;
	// memset(&clnt_addr, 0, sizeof(clnt_addr));
	// clnt_addr.sin6_family = AF_INET6;
	// clnt_addr.sin6_port = htons(4200);
	// ret_val = inet_pton(AF_INET6, "fe80::6017:8fcd:629e:f423", &clnt_addr.sin6_addr);
	// if (ret_val == 0) {
	// 	exit_with_user_msg("inet_pton()", "invalid local address string");
	// } else if (ret_val < 0) {
	// 	exit_with_system_msg("inet_pton() failed");
	// }
	// if (bind(sock, (struct sockaddr *)&clnt_addr, sizeof(clnt_addr)) < 0) {
	// 	exit_with_system_msg("bind() failed");
	// }
	
	


	/* 3. Set the server addresss structure */

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		exit_with_system_msg("connect() failed");
	}
	get_remote_addr(sock);
	get_local_addr(sock);

	receive_greetings(sock);

	size_t echo_strlen = strlen(echo_str);
	ssize_t numbytes_sent = send(sock, echo_str, echo_strlen, 0);
	if (numbytes_sent < 0) {
		exit_with_system_msg("send() failed");
	} else if (numbytes_sent != (ssize_t)echo_strlen) {
		exit_with_user_msg("send()", "Unexpected number of bytes sent");
	}

	char buf[BUF_SIZE];
	ssize_t totalbytes_rcvd = 0;
	while (totalbytes_rcvd < (ssize_t)echo_strlen) {
		ssize_t rcvd_bytes = recv(sock, buf, BUF_SIZE - 1, 0);
		if (rcvd_bytes < 0) {
			exit_with_system_msg("recv() failed");
		}
		buf[rcvd_bytes] = '\0';
		totalbytes_rcvd += rcvd_bytes;
		fputs(buf, stdout);
	}
	fputc('\n', stdout);

	close(sock);

	return 0;
}
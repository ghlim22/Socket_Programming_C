#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/_endian.h>
#include <sys/_types/_in_port_t.h>
#include <sys/_types/_size_t.h>
#include <sys/_types/_ssize_t.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "practical.h"

int main(int argc, char *argv[])
{
	/* 1. Application setup and parameter parsing */
	if (!(argc == 3 || argc == 4)) {
		exit_with_user_msg("Parameter(s)", "<Server Address> <Echo Word> [<Server Port>]");
	}
	/* 2. Socket creation and address setup */
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* Create a reliable, stream socket using IPv4 TCP */
	if (sock < 0) {
		exit_with_system_msg("sock() failed");
	}

	in_port_t serv_port = (argc == 4) ? atoi(argv[3]) : 7; /* 7 is the well-known echo port */
	
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr)); /* Zero out the structure */
	serv_addr.sin_family = AF_INET; /* IPv4 address family */
	serv_addr.sin_port = htons(serv_port);
	int ret_val = inet_pton(AF_INET, argv[1], &serv_addr.sin_addr.s_addr);
	if (ret_val == 0) {
		exit_with_user_msg("inet_pton()", "invalid server address string");
	} else if (ret_val < 0) {
		exit_with_system_msg("inet_pton() failed");
	}

	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		exit_with_system_msg("connect() failed");
	}

	const char* echo_str = argv[2];
	size_t echo_str_len = strlen(echo_str);
	ssize_t num_bytes_sent = send(sock, echo_str, echo_str_len, 0);
	if (num_bytes_sent < 0) {
		exit_with_system_msg("send() failed");
	} else if (num_bytes_sent != (ssize_t)echo_str_len) {
		exit_with_user_msg("send()", "Unexpected number of bytes sent");
	}

	char buf[BUF_SIZE];
	ssize_t total_bytes_rcvd = 0;
	while (total_bytes_rcvd < (ssize_t)echo_str_len) {
		ssize_t rcvd_bytes = recv(sock, buf, BUF_SIZE - 1, 0);
		if (rcvd_bytes < 0) {
			exit_with_system_msg("recv() failed");
		} else if (rcvd_bytes == 0) { /* End of the stream */
			exit_with_user_msg("recv()", "connection closed prematurely");
		}
		buf[rcvd_bytes] = '\0';
		total_bytes_rcvd += rcvd_bytes;
		fputs(buf, stdout);
	}
	fputc('\n', stdout);

	close(sock);

	return 0;
}
#pragma once

#ifndef PRACTICAL_H
#define PRACTICAL_H

#include <stdio.h>
#include <netdb.h>

void exit_with_system_message(const char *msg);
void exit_with_user_message(const char *msg, const char *detail);

struct addrinfo *get_addr_list(const char *host, const char *service,
                               struct addrinfo *hint);
int setup_tcp_server_socket(const char *service);
void print_socket_address(const struct sockaddr *addr, FILE *stream);

int accept_tcp_connection(int serv_sock);

#endif

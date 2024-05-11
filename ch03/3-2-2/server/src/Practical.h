#pragma once

#ifndef PRACTICAL_H
#define PRACTICAL_H

#include <stdio.h>
#include <netdb.h>

void exit_with_system_message(const char *msg);
void exit_with_user_message(const char *msg, const char *detail);

int accept_connection(int serv_sock);
struct addrinfo *get_address_list_or_null(const char *node_or_null,
                                          const char *service,
                                          const struct addrinfo *hints_or_null);
void handle_client(int sock);
void print_socket_address(const struct sockaddr *address, FILE *stream);
int setup_server_socket(const char *service);

#endif

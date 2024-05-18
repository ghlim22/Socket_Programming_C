#pragma once

#ifndef PRACTICAL_H
#define PRACTICAL_H

#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>

void exit_with_system_message(const char *msg);
void exit_with_user_message(const char *msg, const char *detail);

struct addrinfo *get_addr_list(const char *host, const char *service,
                               struct addrinfo *hint);

int setup_tcp_client_socket(const char *host, const char *service);

#endif

#pragma once

#ifndef PRACTICAL_H
#define PRACTICAL_H

#define GREETING_MSG ("Welcome!!")

#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>

void exit_with_system_message(const char *msg);
void exit_with_user_message(const char *msg, const char *detail);

struct addrinfo *get_address_info(const char *name, const char *service,
                                  struct addrinfo *hint);

void print_socket_address(const struct sockaddr *addr, FILE *stream);

#endif

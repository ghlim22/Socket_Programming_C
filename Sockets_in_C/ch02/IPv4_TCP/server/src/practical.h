#pragma once
#ifndef PRACTICAL_H
#define PRACTICAL_H

#include <netinet/in.h>

#define BUF_SIZE (100)
#define MAX_PENDING (5)

void exit_with_user_msg(const char *msg, const char *detail);
void exit_with_system_msg(const char *msg);
void echo(int clnt_sock);
void print_client_info(const struct sockaddr_in *addr);

#endif /* PRACTICAL_H */

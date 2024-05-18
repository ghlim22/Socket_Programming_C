#pragma once
#ifndef PRACTICAL_H
#define PRACTICAL_H

extern const int BUF_SIZE;

void die_with_user_message(const char *msg, const char *detail);
void die_with_system_message(const char *msg);
void handle_tcp_client(int clnt_sock);

#endif /* PRACTICAL_H */

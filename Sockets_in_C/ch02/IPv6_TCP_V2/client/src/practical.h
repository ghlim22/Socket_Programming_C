#pragma once
#ifndef PRACTICAL_H
#define PRACTICAL_H

#define BUF_SIZE (100)
#define MAX_PENDING (5)
#define GREETING_MSG ("hello")

void exit_with_user_msg(const char *msg, const char *detail);
void exit_with_system_msg(const char *msg);
void echo(int clnt_sock);
void receive_greetings(int sock);
void get_remote_addr(int sock);
void get_local_addr(int sock);

#endif /* PRACTICAL_H */

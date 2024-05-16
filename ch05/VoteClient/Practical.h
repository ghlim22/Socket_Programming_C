#pragma once

#define BUF_SIZE (100)

void ExitWithSystemMsg(const char *msg);
void ExitWithUserMsg(const char *msg, const char *detail);

struct addrinfo *GetAddrList(const char *host, const char *service,
                               struct addrinfo *hint);

int SetupClient(const char *host, const char *service);

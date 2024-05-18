#include "Practical.h"
#include <stdio.h>
#include <sys/socket.h>
static const int BUF_SIZE = 10;

void handle_client(int sock)
{
    char buf[BUF_SIZE];
    ssize_t received;
    ssize_t sent;

    while (1) {
        received = recv(sock, buf, BUF_SIZE, 0);
        if (received < 0) {
            exit_with_system_message("recv() failed");
        } else if (received == 0) {
            break;
        }
        sent = send(sock, buf, received, 0);
        if (sent < 0) {
            exit_with_system_message("send() failed");
        } else if (sent != received) {
            exit_with_user_message("send()", "sent unexpected number of bytes");
        }
    }
}
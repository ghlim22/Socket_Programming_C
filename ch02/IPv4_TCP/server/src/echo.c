#include <sys/_types/_ssize_t.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "practical.h"

void echo(int clnt_sock)
{
    char buf[BUF_SIZE];

    ssize_t num_bytes_rcvd = recv(clnt_sock, buf, BUF_SIZE, 0);
    if (num_bytes_rcvd < 0) {
        exit_with_system_msg("recv() failed");
    }

    while (num_bytes_rcvd > 0) {
        ssize_t num_bytes_sent = send(clnt_sock, buf, num_bytes_rcvd, 0);
        if (num_bytes_sent < 0) {
            exit_with_system_msg("send() failed");
        } else if (num_bytes_sent != num_bytes_rcvd) {
            exit_with_user_msg("send()", "sent unexpected number of bytes");
        }

        num_bytes_rcvd = recv(clnt_sock, buf, BUF_SIZE, 0);
        if (num_bytes_rcvd < 0) {
            exit_with_system_msg("recv() failed");
        }
    }
}

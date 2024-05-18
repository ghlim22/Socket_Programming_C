#include "Practical.h"
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void handle_tcp_client(int clnt_sock)
{
    char buf[BUF_SIZE];
    ssize_t num_bytes_rcvd = recv(clnt_sock, buf, BUF_SIZE, 0);
    if (num_bytes_rcvd < 0) {
        die_with_system_message("recv() failed");
    }

    while (num_bytes_rcvd > 0) { /* 0 indicates end of stream */
        ssize_t num_bytes_sent = send(clnt_sock, buf, num_bytes_rcvd, 0);
        if (num_bytes_sent < 0) {
            die_with_system_message("send() failed");
        } else if (num_bytes_sent != num_bytes_rcvd) {
            die_with_user_message("send()", "sent unexpected number of bytes");
        }

        /* See if there is more data to receive */
        num_bytes_rcvd = recv(clnt_sock, buf, BUF_SIZE, 0);
        if (num_bytes_rcvd < 0) {
            die_with_system_message("recv() failed");
        }
    }

    close(clnt_sock);
}

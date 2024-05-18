#include "Practical.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        exit_with_user_message("Parameter(s)", "<Service/Port>");
    }

    int serv_sock = setup_server_socket(argv[1]);
    if (serv_sock < 0) {
        exit_with_user_message("setup_server_socket()", "fail to setup socket");
    }

	while (1) {
		int sock = accept_connection(serv_sock);
		handle_client(sock);
		close(sock);
	}

	close(serv_sock);

    return 0;
}
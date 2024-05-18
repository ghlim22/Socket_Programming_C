#include "Practical.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/_select.h>
#include <sys/_types/_fd_def.h>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  if (argc < 3) {
    ExitWithUserMsg("Parameter(s)", "<Timeout (secs.)> <Port/Service> ...");
  }

  const long timeout = atol(argv[1]);
  const int noPorts = argc - 2;

  int sockets[noPorts];
  int maxDesc = -1;

  for (int i = 0; i < noPorts; ++i) {
    sockets[i] = SetupServer(argv[i + 2]);

    if (sockets[i] > maxDesc) {
      maxDesc = sockets[i];
    }
  }

  puts("Starting server: Hit return to shutdown");
  bool bRunning = true;
  fd_set sockSet;

  while (bRunning) {
    FD_ZERO(&sockSet);
    FD_SET(STDIN_FILENO, &sockSet);
    for (int i = 0; i < noPorts; ++i) {
      FD_SET(sockets[i], &sockSet);
    }

    struct timeval selTimeout;
    selTimeout.tv_sec = timeout;
    selTimeout.tv_usec = 0;

    if (select(maxDesc + 1, &sockSet, NULL, NULL, &selTimeout) == 0) {
      printf("No echo requests for %ld secs... Server still alive\n", timeout);
    } else {
      if (FD_ISSET(STDIN_FILENO, &sockSet)) {
        puts("Shutting down server");
        getchar();
        bRunning = false;
      }

      for (int i = 0; i < noPorts; ++i) {
        if (FD_ISSET(sockets[i], &sockSet)) {
          printf("Request on port %d: ", i);
          HandleClient(AcceptConnection(sockets[i]));
        }
      }
    }
  }

  for (int i = 0; i < noPorts; ++i) {
    close(sockets[i]);
  }

  return 0;
}
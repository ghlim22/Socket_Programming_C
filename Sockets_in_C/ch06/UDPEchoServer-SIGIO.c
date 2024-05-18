#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_size_t.h>
#include <sys/_types/_socklen_t.h>
#include <sys/_types/_ssize_t.h>
#include <sys/fcntl.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE (100)

int sock;

void UseIdleTime(void);
void SIGIOHandler(int signo);
void PrintSocketAddress(struct sockaddr *addr, FILE *stream);

int main(int argc, char *argv[])
{
  if (argc != 2) {
    return 0;
  }

  const char *service = argv[1];

  struct addrinfo addrCriteria;
  memset(&addrCriteria, 0, sizeof(struct addrinfo));
  addrCriteria.ai_family = AF_UNSPEC;
  addrCriteria.ai_flags = AI_PASSIVE;
  addrCriteria.ai_socktype = SOCK_DGRAM;
  addrCriteria.ai_protocol = IPPROTO_UDP;

  struct addrinfo *list;
  int ret = getaddrinfo(NULL, service, &addrCriteria, &list);
  if (ret != 0) {
    fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(ret));
    exit(EXIT_FAILURE);
  }

  sock = socket(list->ai_family, list->ai_socktype, list->ai_protocol);
  if (sock < 0) {
    exit(EXIT_FAILURE);
  }

  if (bind(sock, list->ai_addr, list->ai_addrlen) < 0) {
    exit(EXIT_FAILURE);
  }

  freeaddrinfo(list);
  struct sigaction handler;
  handler.__sigaction_u.__sa_handler = SIGIOHandler;
  if (sigfillset(&handler.sa_mask) < 0) { // Create mask that mask all signals
    exit(EXIT_FAILURE);
  }
  handler.sa_flags = 0; // No flags

  if (sigaction(SIGIO, &handler, NULL) < 0) {
    exit(EXIT_FAILURE);
  }

  /* We must own the socket to receive the SIGIO delivery */
  if (fcntl(sock, F_SETOWN, getpid()) < 0) {
    exit(EXIT_FAILURE);
  }

  /* Arrange for nonblocking IO and SIGIO delivery */
  if (fcntl(sock, F_SETFL, O_NONBLOCK | FASYNC) < 0) {
    exit(EXIT_FAILURE);
  }

  /* Go off and do real work; echoing happens in the background */
  while (1) {
    UseIdleTime();
  }

  return 0;
}

void UseIdleTime(void)
{
  puts(".");
  sleep(3);
}

void SIGIOHandler(int signo)
{
  ssize_t numBytesRcvd;
  do {
    struct sockaddr_storage clntAddr;
    socklen_t clntLen = sizeof(clntAddr);
    char buf[100];

    numBytesRcvd = recvfrom(sock, buf, sizeof(buf), 0,
                            (struct sockaddr *)&clntAddr, &clntLen);
    if (numBytesRcvd < 0) {
      if (errno != EWOULDBLOCK) {
        perror("recvfrom()");
        exit(EXIT_FAILURE);
      }
    } else {
      fprintf(stdout, "Handling client ");
      PrintSocketAddress((struct sockaddr *)&clntAddr, stdout);
      fputc('\n', stdout);

      ssize_t numBytesSent = sendto(sock, buf, numBytesRcvd, 0,
                                    (struct sockaddr *)&clntAddr, clntLen);
      if (numBytesSent < 0) {
        perror("sendto()");
        exit(EXIT_FAILURE);
      } else if (numBytesSent != numBytesRcvd) {
        exit(EXIT_FAILURE);
      }
    }
  } while (numBytesRcvd > 0);
}

void PrintSocketAddress(struct sockaddr *addr, FILE *stream)
{
  assert(addr != NULL && stream != NULL);

  void *numericAddress;
  in_port_t port;

  switch (addr->sa_family) {
  case AF_INET:
    numericAddress = &((struct sockaddr_in *)addr)->sin_addr;
    port = ((struct sockaddr_in *)addr)->sin_port;
    break;
  case AF_INET6:
    numericAddress = &((struct sockaddr_in6 *)addr)->sin6_addr;
    port = ((struct sockaddr_in6 *)addr)->sin6_port;
    break;
  default:
    fputs("[unknown type]", stream);
    return;
  }

  char buf[BUF_SIZE];
  if (inet_ntop(addr->sa_family, numericAddress, buf, sizeof(buf)) == NULL) {
    exit(EXIT_FAILURE);
    fputs("[invalid address]", stream);
  } else {
    fprintf(stream, "%s", buf);
    if (port != 0) {
      fprintf(stream, "-%u", port);
    }
  }
}

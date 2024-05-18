#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_socklen_t.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT ("7777")
#define MAX_PENDING (10)

void *get_in_addr(struct sockaddr *sa)
{
  switch (sa->sa_family) {
  case AF_INET:
    return &((struct sockaddr_in *)sa)->sin_addr;
  case AF_INET6:
    return &((struct sockaddr_in6 *)sa)->sin6_addr;
  default:
    return NULL;
  }
}

int get_listener_socket(void)
{
  int listener;
  int yes = 1;
  int rv;
  struct addrinfo *ai;
  struct addrinfo *p;
  struct addrinfo hints;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;
  if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
    fprintf(stderr, "select server: %s\n", gai_strerror(rv));
    exit(EXIT_FAILURE);
  }

  for (p = ai; p != NULL; p = p->ai_next) {
    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listener < 0) {
      continue;
    }

    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
      close(listener);
      continue;
    }

    break;
  }

  /* If we got here, it means we didn't get bound */
  if (p == NULL) {
    printf("p == NULL\n");
    return -1;
  }

  if (listen(listener, MAX_PENDING) < 0) {
    return -1;
  }

  return listener;
}

void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
  if (*fd_count == *fd_size) {
    *fd_size *= 2;
    *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
  }

  (*pfds)[*fd_count].fd = newfd;
  (*pfds)[*fd_count].events = POLLIN;

  (*fd_count)++;
}

void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
  pfds[i] = pfds[*fd_count - 1];
  (*fd_count)--;
}

int main(void)
{
  int listener;
  int newfd;
  struct sockaddr_storage remoteaddr;
  socklen_t addrlen;
  char buf[256];
  char remoteIP[INET6_ADDRSTRLEN];

  int fd_count = 0;
  int fd_size = 5;
  struct pollfd *pfds = malloc(sizeof(struct pollfd) * fd_size);

  listener = get_listener_socket();
  if (listener == -1) {
    fprintf(stderr, "error getting listening socket\n");
    exit(1);
  }

  pfds[0].fd = listener;
  pfds[0].events = POLLIN;

  fd_count = 1;

  while (1) {
    int poll_count = poll(pfds, fd_count, -1);
    if (poll_count == -1) {
      perror("poll");
      exit(1);
    }

    for (int i = 0; i < fd_count; ++i) {
      if (pfds[i].revents & POLLIN) {
        if (pfds[i].fd == listener) {
          addrlen = sizeof(remoteaddr);
          newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
          if (newfd == -1) {
            perror("accept()");
          } else {
            add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

            printf("pollserver: new connection from %s on socket %d\n",
                   inet_ntop(remoteaddr.ss_family,
                             get_in_addr((struct sockaddr *)&remoteaddr),
                             remoteIP, INET6_ADDRSTRLEN),
                   newfd);
          }
        } else {
          int nbytes = recv(pfds[i].fd, buf, sizeof(buf), 0);
          int sender_fd = pfds[i].fd;

          if (nbytes <= 0) {
            if (nbytes == 0) {
              printf("pollserver: socket %d hung up\n", sender_fd);
            } else {
              perror("recv()");
            }

            close(pfds[i].fd);
            del_from_pfds(pfds, i, &fd_count);

          } else {
            for (int j = 0; j < fd_count; ++j) {
              int dest_fd = pfds[i].fd;
              if (dest_fd != listener) {
                if (send(dest_fd, buf, nbytes, 0) == -1) {
                  perror("send()");
                }
              }
            }
          }
        }
      }
    }
  }

  return 0;
}
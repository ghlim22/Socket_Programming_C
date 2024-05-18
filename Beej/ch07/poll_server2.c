#include <arpa/inet.h>
#include <i386/limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_in_port_t.h>
#include <sys/_types/_socklen_t.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE (256)
#define INIT_FD_SIZE (5)
#define MAX_PENDING (10)
#define PORT ("4242")

void *get_in_addr(struct sockaddr *sa)
{
  switch (sa->sa_family) {
  case AF_INET:
    return &((struct sockaddr_in *)&sa)->sin_addr;
  case AF_INET6:
    return &((struct sockaddr_in6 *)&sa)->sin6_addr;
  default:
    return NULL;
  }
}

struct addrinfo *get_ai_list(const char *node, const char *service,
                             const struct addrinfo *hints)
{
  int rv;
  struct addrinfo *ai;

  rv = getaddrinfo(node, service, hints, &ai);
  if (rv != 0) {
    fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(rv));
    exit(EXIT_FAILURE);
  }
  return ai;
}

void config_ai_hints(struct addrinfo *hints)
{
  memset(hints, 0, sizeof(struct addrinfo));
  hints->ai_family = AF_UNSPEC;
  hints->ai_socktype = SOCK_STREAM;
  hints->ai_protocol = IPPROTO_TCP;
  hints->ai_flags = AI_PASSIVE;
}

struct addrinfo *get_ai_passive(const char *service)
{
  struct addrinfo hints;
  struct addrinfo *list;

  config_ai_hints(&hints);
  list = get_ai_list(NULL, service, &hints);
  return list;
}

int get_binded_socket(const char *service)
{
  int yes = 1;
  int sock;
  struct addrinfo *p;
  struct addrinfo *list;

  list = get_ai_passive(service);
  for (p = list; p != NULL; p = p->ai_next) {
    sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sock < 0) {
      continue;
    }
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (bind(sock, p->ai_addr, p->ai_addrlen) < 0) {
      close(sock);
      continue;
    }
    break;
  }
  freeaddrinfo(list);
  if (p == NULL) {
    perror("bind()");
    exit(EXIT_FAILURE);
  }
  return sock;
}

int get_listener_socket(const char *service)
{
  int sock;

  sock = get_binded_socket(service);
  if (listen(sock, MAX_PENDING) < 0) {
    perror("listen()");
    exit(EXIT_FAILURE);
  }
  return sock;
}

void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
  if (*fd_count == *fd_size) {
    *fd_size *= 2;
    *pfds = realloc(*pfds, *fd_size * sizeof(struct pollfd));
  }
  (*pfds)[*fd_count].fd = newfd;
  (*pfds)[*fd_count].events = POLLIN;
  *fd_count += 1;
}

void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
  pfds[i] = pfds[*fd_count - 1];
  *fd_count -= 1;
}

void print_addr_info(const struct sockaddr *sa)
{
  char buf[INET6_ADDRSTRLEN];
  in_port_t port;
  void *numeric_addr;

  switch (sa->sa_family) {
  case AF_INET:
    numeric_addr = &((struct sockaddr_in *)sa)->sin_addr;
    port = ((struct sockaddr_in *)sa)->sin_port;
    break;
  case AF_INET6:
    numeric_addr = &((struct sockaddr_in6 *)sa)->sin6_addr;
    port = ((struct sockaddr_in6 *)sa)->sin6_port;
    break;
  default:
    printf("[unknown address family]\n");
    break;
  }
  if (inet_ntop(sa->sa_family, numeric_addr, buf, sizeof(buf)) == NULL) {
    printf("[address parse error]\n");
  } else {
    printf("%s:%d", buf, port);
  }
}

void accept_conn(struct pollfd *pfds[], int *fd_count, int *fd_size,
                 int listener)
{
  struct sockaddr_storage ss;
  socklen_t ss_len = sizeof(ss);

  int newfd = accept(listener, (struct sockaddr *)&ss, &ss_len);
  if (newfd < 0) {
    perror("accept()");
    exit(EXIT_FAILURE);
  }
  add_to_pfds(pfds, newfd, fd_count, fd_size);
  printf("new connection: ");
  print_addr_info((struct sockaddr *)&ss);
  putc('\n', stdout);
}

void handler(struct pollfd pfds[], int *fd_count, int i)
{
  char buf[BUF_SIZE];
  int sender = pfds[i].fd;

  int nbytes = recv(sender, buf, sizeof(buf), 0);
  if (nbytes <= 0) {
    if (nbytes < 0) {
      perror("recv()");
    } else {
      printf("socket %d hung up\n", sender);
    }
    close(sender);
    del_from_pfds(pfds, i, fd_count);
  } else {
    // echo
    nbytes = send(sender, buf, nbytes, 0);
    if (nbytes < 0) {
      perror("send()");
    }
  }
}

int main(void)
{
  struct pollfd *pfds = malloc(sizeof(struct pollfd) * INIT_FD_SIZE);
  int fd_count = 0;
  int fd_size = INIT_FD_SIZE;

  int listener = get_listener_socket(PORT);
  pfds[0].fd = listener;
  pfds[0].events = POLLIN;
  fd_count++;

  while (1) {
    int poll_count = poll(pfds, fd_count, -1);
    if (poll_count < 0) {
      perror("poll()");
      exit(1);
    }
    for (int i = 0; i < fd_count; ++i) {
      if (pfds[i].revents & POLLIN) {
        if (pfds[i].fd == listener) {
          accept_conn(&pfds, &fd_count, &fd_size, listener);
        } else {
          handler(pfds, &fd_count, i);
        }
      }
    }
  }

  return 0;
}

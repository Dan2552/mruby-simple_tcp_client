#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

int lookup_host(const char *host, char *ip_address) {
  struct addrinfo hints;
  struct addrinfo *res;
  struct addrinfo *result;
  int errcode;
  void *ptr;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags |= AI_CANONNAME;

  errcode = getaddrinfo(host, NULL, &hints, &result);
  if (errcode != 0) {
    perror("getaddrinfo");
    return -1;
  }

  res = result;

  while(res) {
    inet_ntop(res->ai_family, res->ai_addr->sa_data, ip_address, 100);

    switch(res->ai_family) {
    case AF_INET:
      ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
      break;
    }
    inet_ntop(res->ai_family, ptr, ip_address, 100);

    res = res->ai_next;
  }

  freeaddrinfo(result);

  return 0;
}

int main(void) {
  int result;
  char *ip_address;

  result = lookup_host("rebaseface.com", ip_address);
  printf("%s", ip_address);
}

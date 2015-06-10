// Musinsky Jan
// 2015-06-15

#include "vmemonitor_client.h"

int connectServer(const char *host, const char *port)
{
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int ret, sfd;

  // make sure all fields in hints structure are NULL
  memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_UNSPEC;   // IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP

  // obtain address(es) list matching host/port
  ret = getaddrinfo(host, port, &hints, &result);
  if (ret != 0) {
    fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(ret));
    return -1;
  }

  // loop through address(es) list until we successfully connect
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (sfd == -1) continue; // try next address

    if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) break; // success
    close(sfd); // failed
  }

  freeaddrinfo(result);
  if (rp == NULL) {
    fprintf(stderr, "no address succeeded connect\n");
    return -1;
  }

  return sfd;
}

void closeConnect(int sfd)
{
  if (shutdown(sfd, SHUT_WR) == -1) perror("shutdown failed");
  if (close(sfd) == -1) perror("close socket failed");
}

int sendNotify(int sfd, const char *buffer)
{
  if (buffer == NULL) {
    fprintf(stderr, "no buffer\n");
    closeConnect(sfd);
    return -1;
  }

  size_t lenbytes = strlen(buffer);
  ssize_t sentbytes = send(sfd, buffer, lenbytes, 0);
  if (sentbytes == -1) perror("send failed");

  closeConnect(sfd);
  if (sentbytes != (ssize_t)lenbytes) {
    fprintf(stderr, "send not deliver all bytes\n");
    return -1;
  }

  return 0;
}

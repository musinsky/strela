// Musinsky Jan
// 2016-06-20

#include "vmemonitor_client.h"
#include "vmemonitor.h"

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

  // set timeout value => sendfile failed: Resource temporarily unavailable
  struct timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  if (setsockopt(sfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) == -1)
    perror("setsockopt failed");

  if (vmode) {
    printTime();
    fprintf(stderr, "connect \t %s:%s\n", host, port);
  }

  return sfd;
}

void closeConnect(int sfd)
{
  if (sfd == -1) return;
  if (shutdown(sfd, SHUT_WR) == -1) perror("shutdown failed");
  if (close(sfd) == -1) perror("close socket failed");

  if (vmode) {
    printTime();
    fprintf(stderr, "close socket\n");
  }
}

int sendBuffer(int sfd, const char *buffer)
{
  if (buffer == NULL) {
    fprintf(stderr, "no buffer\n");
    return -1;
  }

  size_t lenbytes;   // size_t  {aka long unsigned int}
  ssize_t sentbytes; // ssize_t {aka long int}

  lenbytes = strlen(buffer);
  sentbytes = send(sfd, buffer, lenbytes, 0);
  if (sentbytes == -1) perror("send failed");

  if (sentbytes != (ssize_t)lenbytes) {
    fprintf(stderr, "sendBuffer not deliver all bytes\n");
    return -1;
  }

  return 0;
}

int sendHeader(int sfd, int code, long datafrom, long datato, const char *fname)
{
  // header always has a fixed size (BUF_HEAD_SIZE = 280 bytes)

  char buf[BUF_HEAD_SIZE];
  size_t lenbytes;
  ssize_t sentbytes;

  if (fname == NULL)            fname = "null";
  if (strlen(fname) > NAME_MAX) fname = "incorrect";
  memset(buf, 0, sizeof(buf));

  lenbytes = 0;
  memcpy(buf, &code, sizeof(code));                    // code info
  lenbytes += sizeof(code);
  memcpy(buf + lenbytes, &datafrom, sizeof(datafrom)); // data from
  lenbytes += sizeof(datafrom);
  memcpy(buf + lenbytes, &datato, sizeof(datato));     // data to
  lenbytes += sizeof(datato);
  memcpy(buf + lenbytes, fname, strlen(fname));        // data fname
  lenbytes += NAME_MAX + 1;
  memcpy(buf + lenbytes, &headID, sizeof(headID));     // header ID (+ buffer size)
  lenbytes += sizeof(headID);

  sentbytes = send(sfd, buf, BUF_HEAD_SIZE, 0);
  if (sentbytes == -1) perror("send failed");

  if (sentbytes != (ssize_t)lenbytes) {
    fprintf(stderr, "sendHeader not deliver all bytes\n");
    return -1;
  }

  return 0;

  /* endian problem, dont send number (short, integer) directly (only with htonl function)
   * anything in C can be read or written as an array of char
   * int value = 12345
   * char *data = (char *)&value
   * int test
   * memcpy(&test, data, sizeof(value))
   */
}

long sendFile(int sfd, off_t *foffset, FILE *fstream, const char *fname)
{
  // sendfile over socket (zero-copy) from foffset to end of the file
  // return the total number of bytes sent
  // foffset will be set to the offset of the byte following
  // the last byte that was read (file size, next offset)

  if (fstream == NULL) {
    fprintf(stderr, "no fstream\n");
    return -1;
  }
  if (foffset == NULL) {
    fprintf(stderr, "no foffset\n");
    return -1;
  }

  size_t lenbytes;   // size_t  {aka long unsigned int}
  ssize_t sentbytes; // ssize_t {aka long int}
  off_t filepos;     // off_t   {aka long int}
  int fid;

  fid = fstream->_fileno;
  filepos = lseek(fid, 0, SEEK_END);
  if (filepos == -1) {
    perror("lseek SEEK_END failed");
    return -1;
  }
  if ((*foffset) > filepos) {
    fprintf(stderr, "offset: %ld > file size: %ld\n", *foffset, filepos);
    return -1;
  }
  lenbytes = filepos;

  sendHeader(sfd, 0, *foffset, filepos, fname); // code info temporary not used

  filepos = lseek(fid, *foffset, SEEK_SET);
  if (filepos == -1) {
    perror("lseek SEEK_SET failed");
    return -1;
  }
  lenbytes -= filepos;

  if (vmode) {
    printTime();
    fprintf(stderr, "sendfile from \t %ld\n", *foffset);
  }

  // sendfile will transfer at most 0x7FFFF000 (2.147.479.552) bytes
  // maximal write size of INT_MAX & PAGE_CACHE_MASK (2^31-1-4095)
  size_t totalsent;

  totalsent = 0;
  while(totalsent < lenbytes) {
    // foffset is adjusted to reflect the number of bytes read from fid
    sentbytes = sendfile(sfd, fid, foffset, lenbytes - totalsent);
    if (sentbytes == -1) {
      perror("sendfile failed");
      return -1;
    }

    totalsent += sentbytes;
    //    if (vmode) {
    //      printTime();
    //      fprintf(stderr, "sendfile \t\t %ld\n", totalsent);
    //    }
  }

  if (totalsent != lenbytes)
    fprintf(stderr, "sendFile not deliver all bytes\n");

  if (vmode) {
    printTime();
    fprintf(stderr, "sendfile to \t %ld (total %ld)\n", *foffset, totalsent);
  }

  return totalsent;

  /* Default TCP uses Nagle's algorithm to collect small outgoing packets to send
   * all at once (effect on latency, not optimal for streams). Useful to employ
   * the TCP_CORK option to minimize the number of packets and to tune performance.
   *
   * Recommend setting the TCP_CORK option when you're sure that you will be sending
   * multiple data sets together (such as header and a body of HTTP response), with
   * no delays between them.
   *
   * header + bigdata + header + bigdata + header + bigdata + ... => TCP_CORK important
   * header + big data (close connection) = our case => TCP_CORK unimportant (probably)
   */
}

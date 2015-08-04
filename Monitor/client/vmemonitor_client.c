// Musinsky Jan
// 2015-08-05

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
    fprintf(stderr, "sendNotify not deliver all bytes\n");
    return -1;
  }

  return 0;
}

void sendInfo(int sfd, int info, long datafrom, long datato, const char *fname)
{
  // TODO: testing
  char buf[BUF_INFO_SIZE];

  size_t lenbytes;
  ssize_t sentbytes;

  lenbytes = 0;
  memcpy(buf, &info, sizeof(info));                    // info code
  lenbytes += sizeof(info);
  memcpy(buf + lenbytes, &datafrom, sizeof(datafrom)); // data from
  lenbytes += sizeof(datafrom);
  memcpy(buf + lenbytes, &datato, sizeof(datato));     // data to
  lenbytes += sizeof(datato);
  memcpy(buf + lenbytes, fname, strlen(fname));        // data fname
  lenbytes += strlen(fname);

  sentbytes = send(sfd, buf, lenbytes, 0);
  if (sentbytes == -1) perror("send failed");

  if (sentbytes != (ssize_t)lenbytes)
    fprintf(stderr, "sendInfo not deliver all bytes\n");

  //  // Write int value to socket
  //  void write_int(int sock, int value) {
  //    const char *data = (const char *)&value;
  //    write_range(sock, data, data + sizeof(int));
  //  }

  // dont send number (short, integer) directly (endian problem) only with htonl function
  // http://stackoverflow.com/questions/13279024/convert-a-uint16-t-to-char2-to-be-sent-over-socket-unix
  // http://stackoverflow.com/questions/16729496/data-transfer-across-big-and-small-endian-machines

  /* receiving
  // anything in C can be read or written as an array of char
  // int a = 1;
  // char *c = (char*)(&a);

  lenbytes = 0;
  memcpy(&info,     &buf[lenbytes], sizeof(info));
  lenbytes += sizeof(info);
  memcpy(&datafrom, &buf[lenbytes], sizeof(datafrom));
  lenbytes += sizeof(datafrom);
  memcpy(&datato,   &buf[lenbytes], sizeof(datato));
  lenbytes += sizeof(datato);
  memcpy(fname,     &buf[lenbytes], NAME_MAX + 1);
  // or
  memcpy(&fname[0], &buf[lenbytes], NAME_MAX + 1);
   */
}

long sendFile(int sfd, off_t *foffset, FILE *fstream)
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
}





long sendFileOLD(int sfd, long foffset, FILE *fstream)
{
  // send file over socket from foffset to end of the file
  // return last sended position (next offset) in bytes of the fstream

  if (fstream == NULL) {
    fprintf(stderr, "no fstream\n");
    return -1;
  }

  size_t nread, wordsize, blocksize;
  ssize_t nsent;
  long filepos, totread, totsent;

  // na rychlost fread vacsia velkost nema vplyv
  // preverit aky ma vplyv velkost na rychlost send
  // share from vmemonitor_file.h (define BLOCK_SIZE) alebo extra
  blocksize = 4096;

  // TODO citam po int (4bytes) citat jednoducho po (1bytes)
  // findLastType je potrebny VME aligned, checkujem SHDR alebo EHDR
  // tu je tato podmienka zbytona (VME aligned)
  // posielat mozem akykolvek file, nie len delitelny 4

  // share from vmemonitor_file.h (define WORDSIZE) alebo NE
  wordsize = sizeof(uint32_t); // minimal word size of data is 4 bytes (32 bits)
  uint32_t buf[blocksize];     // buffer of 4*BLOCK_SIZE = 16384 bytes size

  if (fseek(fstream, foffset, SEEK_SET) != 0) {
    perror("fseek (sendFile) failed");
    return -1;
  }

  filepos = ftell(fstream);
  if (filepos == -1) {
    perror("ftell (sendFile) failed");
    return -1;
  }
  if ((filepos % wordsize) != 0) { // VME specifikum
    fprintf(stderr, "size of file (VME data) not divisible by %lu\n", wordsize);
    return -1;
  }

  totread = 0; // in bytes/wordsize
  totsent = 0; // in bytes

  do {
    nread = fread(buf, wordsize, blocksize, fstream);
    if (nread == 0) {
      printf("nread = 0\n");
      break;
    }

    // sendfile() len s regularnym fileom, opatrne ak je to tmpfs
    // http://stackoverflow.com/questions/13215656/c-sendfile-and-send-difference
    // na konci zas hovoria ze ten sendfile az take vyhodu nema (s malym chunk)
    // So, if the image you want to send to a client is in a file, just go for a sendfile.
    // If you have it in memory (because you processed it somehow, or just generated),
    // use the approach I mentioned earlier.

    nsent = send(sfd, buf, nread*wordsize, 0);
    if (nsent == -1) {
      perror("send (sendFile) failed");
      return -1;
    }
    if (nsent == 0)
      fprintf(stderr, "send (transfer) 0 bytes\n");

    totread += nread;
    totsent += nsent;
  } while (nread == blocksize);

  totread *= wordsize; // to bytes
  if (totread != totsent) printf("%ld (totread) != %ld (totsent)\n", totread, totsent);

  if ((foffset + totread) != ftell(fstream)) printf("%ld != %ld\n", (foffset + totread), ftell(fstream));

  // vraj, ze chvilu pockat so zatvorenim, nech server stihne medzi tym zapisat do file
  // fwrite na server strane zabera urcity cas

  printf("tot read = %ld\n", totread);
  printf("tot sent = %ld\n", totsent);

  return ftell(fstream);  // foofset + totread
}



// takze predsa len na file, ktory sa cita zo suboru je asi najlpesie sendfile, neni to general way (only linux)
// !!! http://www.linuxcenter.ru/lib/articles/programming/sendfile.phtml !!!

// http://stackoverflow.com/questions/13215656/c-sendfile-and-send-difference


// http://blog.superpat.com/2010/06/01/zero-copy-in-linux-with-sendfile-and-splice/
// splice as recvfile


// Nagle not optimal for streams

// https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_MRG/1.2/html/Realtime_Tuning_Guide/sect-Realtime_Tuning_Guide-Application_Tuning_and_Deployment-TCP_NODELAY_and_Small_Buffer_Writes.html
// As discussed briefly in Transmission Control Protocol (TCP), by default TCP uses Nagle's algorithm to collect small
// outgoing packets to send all at once. This can have a detrimental effect on latency.

// Using TCP_NODELAY and TCP_CORK to improve network latency

// http://stackoverflow.com/questions/22124098/is-there-any-significant-difference-between-tcp-cork-and-tcp-nodelay-in-this-use

// The TCP_NODELAY option allows to bypass Naggle, and then send the data as soon as it’s available.
// https://t37.net/nginx-optimization-understanding-sendfile-tcp_nodelay-and-tcp_nopush.html

// sending streaming data, disabling Nagle's algorithm is a better option. !!!!!!!!!!!!
// so TCP_NODELAY is used for disabling nagle's algorithm.

// TCP_CORK aggressively accumulates data.

//long tester = 1234567890123456789;
//char buffer[8];
//memcpy(buffer, &tester, 8);
//long tested;
//memcpy(&tested, buffer, 8);

//root [0] long tester = 123456789012;
//root [1] char *data = (char *)&tester
//root [4] long tested
//root [5] memcpy(&tested, data, 8)

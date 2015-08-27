// Musinsky Jan
// 2015-08-27

#ifndef VMEMONITOR_CLIENT_H
#define VMEMONITOR_CLIENT_H

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netdb.h>
#include <unistd.h>
#include <limits.h>

#define BUF_HEAD_SIZE (2*sizeof(int) + 2*sizeof(long) + NAME_MAX + 1) // 2*4 + 2*8 + 255 + 1 = 280
static const int headID = 987654321 + BUF_HEAD_SIZE;

int connectServer(const char *host, const char *port);

void closeConnect(int sfd);

int sendBuffer(int sfd, const char *buffer);

int sendHeader(int sfd, int code, long datafrom, long datato, const char *fname);

long sendFile(int sfd, off_t *foffset, FILE *fstream, const char *fname);

#endif

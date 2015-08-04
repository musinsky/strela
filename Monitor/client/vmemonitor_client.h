// Musinsky Jan
// 2015-08-05

#ifndef VMEMONITOR_CLIENT_H
#define VMEMONITOR_CLIENT_H

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netdb.h>
#include <unistd.h>
#include <limits.h>

#define BUF_INFO_SIZE (NAME_MAX + 1 + sizeof(int) + 2*sizeof(long)) // 276

int connectServer(const char *host, const char *port);

void closeConnect(int sfd);

void sendInfo(int sfd, int info, long datafrom, long datato, const char *fname);

int sendBuffer(int sfd, const char *buffer);

long sendFile(int sfd, off_t *foffset, FILE *fstream);

long sendFileOLD(int sfd, long foffset, FILE *fstream);

#endif

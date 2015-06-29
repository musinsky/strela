// Musinsky Jan
// 2015-06-24

#ifndef VMEMONITOR_CLIENT_H
#define VMEMONITOR_CLIENT_H

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <limits.h>

int connectServer(const char *host, const char *port);

void closeConnect(int sfd);

void sendInfo(int sfd, int info, long datafrom, long datato, const char *fname);

int sendNotify(int sfd, const char *buffer);

long sendFile(int sfd, long foffset, FILE *fstream);

#endif

// Musinsky Jan
// 2015-06-15

#ifndef VMEMONITOR_CLIENT_H
#define VMEMONITOR_CLIENT_H

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int connectServer(const char *host, const char *port);

void closeConnect(int sfd);

int sendNotify(int sfd, const char *buffer);

#endif

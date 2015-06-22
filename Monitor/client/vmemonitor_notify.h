// Musinsky Jan
// 2015-06-15

#ifndef VMEMONITOR_NOTIFY_H
#define VMEMONITOR_NOTIFY_H

#include <stdio.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <limits.h>

#define EVENT_SIZE (sizeof(struct inotify_event))     // 16
#define BUF_SIZE   (10 * (EVENT_SIZE + NAME_MAX + 1)) // 2720

int watchDir(const char *dname);

void closeWatch(int nfd);

int waitFile(int nfd, char *fname);

#endif

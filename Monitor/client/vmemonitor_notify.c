// Musinsky Jan
// 2015-06-24

#include "vmemonitor_notify.h"

int watchDir(const char *dname)
{
  int nfd = inotify_init();
  if (nfd == -1) {
    perror("inotify_init failed");
    return -1;
  }

  //  $ inotifywait -m --timefmt "%H:%M:%S" --format "%T => %e   %w%f" /data
  //  int wd = inotify_add_watch(nfd, dname, IN_ALL_EVENTS);
  int wd = inotify_add_watch(nfd, dname, IN_CLOSE_WRITE | IN_MOVED_TO);
  if (wd == -1) {
    perror("inotify_add_watch failed");
    return -1;
  }

  return nfd;
}

void closeWatch(int nfd)
{
  // if (inotify_rm_watch(nfd, wd) == -1) perror("inotify_rm_watch failed");
  if (close(nfd) == -1) perror("close inotify failed");
}

int waitFile(int nfd, char *fname)
{
  if (fname == NULL) {
    fprintf(stderr, "no fname\n");
    closeWatch(nfd);
    return -1;
  }

  ssize_t nread;
  char buf[BUF_SIZE];
  char *bp;
  struct inotify_event *event;

  // waiting for events
  nread = read(nfd, buf, BUF_SIZE);
  if (nread == -1) {
    perror("read failed");
    closeWatch(nfd);
    return -1;
  }
  if (nread == 0) {
    fprintf(stderr, "inotify read returns 0\n");
    return 0;
  }

  // loop over all events in the buffer returned by read
  for (bp = buf; bp < (buf + nread); bp += EVENT_SIZE + event->len) {
    event = (struct inotify_event *)bp;

    // events sent by the kernel (from any watch)
    if (event->mask & IN_IGNORED) {
      printf("IN_IGNORED\n");
      closeWatch(nfd);
      return -1;
    }
    if (event->mask & IN_Q_OVERFLOW) {
      printf("IN_Q_OVERFLOW\n");
      return 0;
    }
    // special flags, i.e. IN_ISDIR
    // events (IN_CLOSE_WRITE | IN_MOVED_TO) occur only for files (not dir)

    // watching events (IN_CLOSE_WRITE | IN_MOVED_TO)
    if (event->len == 0) {
      fprintf(stderr, "length of inotify event name is 0\n");
      return 0;
    }
    strcpy(fname, event->name);
    return event->mask; // first (watching) event
  }

  fprintf(stderr, "something wrong in waitFile\n");
  return 0;
}

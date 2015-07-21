// Musinsky Jan
// 2015-07-21

#include "vmemonitor.h"
#include "vmemonitor_notify.h"
#include "vmemonitor_file.h"
#include "vmemonitor_client.h"

int vmode = 0;

void printUsage(const char *name)
{
  fprintf(stdout, "Usage:\n %s [options] monitor-dir monitor-ext server port\n", name);
  fprintf(stdout, "Options:\n");
  fprintf(stdout, "  -v \t\t verbose mode\n");
  fprintf(stdout, "  -h \t\t display this help and exit\n");
  fprintf(stdout, "Example:\n %s -v /data_0 .dat strela.jinr.ru 7503\n", name);
  exit(EXIT_FAILURE);
}

void printTime(void)
{
  time_t t;
  struct tm *lt;

  t = time(NULL);
  lt = localtime(&t);
  if (lt == NULL) {
    perror("localtime failed");
    return;
  }

  fprintf(stderr, "%02d:%02d:%02d ", lt->tm_hour, lt->tm_min, lt->tm_sec);
}

int main(int argc, char *argv[])
{
  // TODO data send over TCP socket (use sendfile() instead send())
  // TODO timeout(~10s), send test signal after no new data

  int opt;
  while ((opt = getopt(argc, argv, "hv")) != -1) {
    switch (opt) {
      case 'v':
        vmode = 1;
        break;
      default: // 'h'
        printUsage(argv[0]);
    }
  }

  if ((argc - optind) != 4) printUsage(argv[0]);
  // skip all options arguments
  argc -= optind;
  argv += optind;

  int fmask, notifyfd, socketfd;
  char datafname[NAME_MAX + 1]; // 256
  long offsetdata, tmp;
  FILE *fdata;

  notifyfd = watchDir(argv[0]);
  if (notifyfd == -1) exit(EXIT_FAILURE);

  offsetdata = -1; // must be -1
  socketfd = -1;   // must be -1
  fdata = NULL;

  for (;;) {
    closeConnect(socketfd);
    socketfd = -1;
    closeFile(fdata);
    fdata = NULL;

    if (vmode) {
      printTime();
      fprintf(stderr, "####### monitoring %s/*%s #######\n", argv[0], argv[1]);
    }

    fmask = waitFile(notifyfd, datafname);
    if (fmask == -1) {
      closeWatch(notifyfd);
      exit(EXIT_FAILURE);
    }
    if (fmask == 0) continue;

    if (endsWith(datafname, argv[1]) == -1) continue;

    if ((anotherName(datafname) == 0) && (offsetdata != -1)) offsetdata = 0; // another file
    if (fmask & IN_MOVED_TO) /* mv vme.dat.tmp vme.dat */    offsetdata = 0; // recycle file

    socketfd = connectServer(argv[2], argv[3]);
    if (socketfd == -1) { // lost connection
      offsetdata = -1;
      continue;
    }

    fdata = openFile(argv[0], datafname);
    if (fdata == NULL) continue;      // maybe sent any log message

    // find last spill after lost connection (or first time), but not for recycle
    if (offsetdata == -1) {
      offsetdata = findLastType(fdata, VME_SHDR, 1);
      if (offsetdata == -1) continue; // maybe sent any log message
    }

    // sendNotify(socketfd, datafname);
    // close connect + close file

    if (vmode) {
      printTime();
      fprintf(stderr, "send from \t %ld\n", offsetdata);
    }
    //tmp = offsetdata;
    offsetdata = sendFile(socketfd, offsetdata, fdata);

    if (vmode) {
      printTime();
      fprintf(stderr, "send to \t %ld\n", offsetdata);
    }
    //printf("data from: %12ld to: %12ld\n", tmp, offsetdata);
  }

  exit(EXIT_SUCCESS);
}

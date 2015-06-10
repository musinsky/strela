// Musinsky Jan
// 2015-06-15

#include "vmemonitor.h"
#include "vmemonitor_notify.h"
#include "vmemonitor_client.h"

int main(int argc, char *argv[])
{
  if (argc != 5) {
    fprintf(stderr, "Usage:\n %s monitor-dir monitor-ext server port\n", argv[0]);
    fprintf(stderr, "Example:\n %s /data .dat strela.jinr.ru 7503\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int notifyfd, socketfd;
  char datafile[NAME_MAX + 1]; // 256
  int wf;

  notifyfd = watchDir(argv[1]);
  if (notifyfd == -1) exit(EXIT_FAILURE);

  for (;;) {
    wf = waitFile(notifyfd, datafile);
    if (wf == -1) {
      closeConnect(socketfd);
      exit(EXIT_FAILURE);
    }
    if (wf == 0) continue;

    if (endsWith(datafile, argv[2]) == -1) continue;
    printf("datafile: %s\n", datafile);

    socketfd = connectServer(argv[3], argv[4]);
    if (socketfd == -1) continue;
    sendNotify(socketfd, datafile);
  }

  exit(EXIT_SUCCESS);
}

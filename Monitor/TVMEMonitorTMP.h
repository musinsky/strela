// @Author  Jan Musinsky <musinsky@gmail.com>
// @Date    18 Jun 2015

#ifndef STRELA_TVMEMonitor
#define STRELA_TVMEMonitor

#include <TObject.h>

class TServerSocket;

class TVMEMonitor : public TObject {

public:
  TVMEMonitor();
  virtual ~TVMEMonitor();

  TServerSocket *GetServSock() const { return fServSock; }
  void           InitServer(Int_t port = 7503);
  TServerSocket       *GetDaco() const { return fDaco; }
  static void          RecvData();
  static void         *ttt(void *args);
  void                 daco();

private:
  static TServerSocket    *fServSock;   // server socket
  TServerSocket           *fDaco;       // server socket

  ClassDef(TVMEMonitor, 1) // VMEMonitor class
};

#endif

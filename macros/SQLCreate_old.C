// root[] .L macros/SQLCreate.C+
// root[] SQLCreate("layers, head"); > seans_layers.sql
// root[] SQLCreate("channels, head"); > seans_channels.sql

// Author: Jan Musinsky
// 16/05/2008

#include <TSystem.h>

#include "TStrawCham.h"

TStrawCham strawCham;

const Int_t MAXL = 8;
const Int_t MAXP = 2;

class Chamber {
private:
  static Int_t total;
  Int_t        nl;                // number of layers in chamber
  Int_t        np;                // number of part(s) in one layer
  Int_t        numb[MAXL];        // unique number of layer
  Int_t        begin[MAXL][MAXP]; // first channel
  Int_t        end[MAXL][MAXP];   // last channel
  Int_t        tracker;           // tracker number
  TString      name[MAXL];        // name of layer
  Double_t     geo[5];            // z0, dz, shift, delta, range

public:
  Chamber();
  void     SetNLayers(Int_t n) { (n > MAXL) ? nl = MAXL : nl = n; }
  void     SetChannels(Bool_t isEnd, ...);
  void     SetTracker(Int_t t) { tracker = t; }
  void     SetNames(const char *prefix, ...);
  void     SetGeometry(Double_t g0, ...);
  void     PrintLayers(Option_t *option = "");
  void     PrintChannels(Option_t *option = "");
};

Int_t Chamber::total = 1; // numb start from 1

Chamber::Chamber()
{
  nl = 0; np = 0; memset(numb, 0, sizeof(numb));
  memset(begin, 0, sizeof(begin)); memset(end, 0, sizeof(end));
  tracker = 0; memset(geo, 0, sizeof(geo));
}
void Chamber::SetChannels(Bool_t isEnd, ...)
{
  if (np > MAXP) {
    Printf("maximum number of parts = %d", MAXP);
    return;
  }

  Int_t channel;
  va_list ap;
  va_start(ap, isEnd);

  for (Int_t i = 0; i < nl; i++) {
    channel = va_arg(ap, Int_t);
    if (isEnd) {
      end[i][np] = channel;
      if (np == 0) numb[i] = total++;
    }
    else
      begin[i][np] = channel;
    //    Printf("%3d", (isEnd) ? end[i][np] : begin[i][np]);
  }

  va_end(ap);
  if (isEnd) np++;
}
void Chamber::SetNames(const char *prefix, ...)
{
  va_list ap;
  va_start(ap, prefix);

  for (Int_t i = 0; i < nl; i++) {
    name[i] = Form("%s%s", prefix, va_arg(ap, char*));
    //    Printf("'%s'", name[i].Data());
  }

  va_end(ap);
}
void Chamber::SetGeometry(Double_t g0, ...)
{
  va_list ap;
  va_start(ap, g0);
  geo[0] = g0;

  for (Int_t i = 1; i < 5; i++) geo[i] = va_arg(ap, Double_t);

  va_end(ap);
}
void Chamber::PrintLayers(Option_t *option)
{
  Int_t nchan;
  const char *dotDash = ",";
  for (Int_t il = 0; il < nl; il++) {
    nchan = 0;
    for (Int_t ip = 0; ip < np; ip++)
      nchan += end[il][ip] - begin[il][ip] + 1;
    if (!strcmp(option, "last") && il == (nl - 1)) dotDash = ";";

    Printf("(%3d, %2d, %10s, %3d, %8.4f, %8.4f, %8.4f, %8.4f, %2d)%s",
	   numb[il], tracker, Form("'%s'", name[il].Data()), nchan,
	   geo[0] + il*geo[1], geo[2], geo[3], geo[4], 1, dotDash);
  }
}
void Chamber::PrintChannels(Option_t *option)
{
  if (!strawCham.GetChannel()) strawCham.HPTDCDecode(); // only once
  Int_t *channel = strawCham.GetChannel();
  Int_t nchan, first;
  const char *dotDash = ",";

  for (Int_t il = 0; il < nl; il++) {
    if (il != 0) Printf("");
    nchan = 0;
    for (Int_t ip = 0; ip < np; ip++) {
      first = begin[il][ip];
      while (first < (end[il][ip] + 1)) {
	if (!strcmp(option, "last") && il == (nl - 1) && ip == (np - 1) &&
	    (first == (end[il][ip] + 1 - 1))) dotDash = ";";
	Printf("(%5d /*%3d*/, %3d /*%10s*/, %3d, %2d, %2d)%s",
	       channel[first], first, numb[il], name[il].Data(),
	       ++nchan, 0, 0, dotDash);
	first++;
      }
    }
  }
}

void SQLCreate(Option_t *option = "layers, channels, head")
{
  Chamber::total = 1; // reset number of lyers
  Int_t nc = 0; // counter
  Chamber ch[99];

  // 2007_03
  // module 0, socket 0 (cable 1y)
  ch[nc].SetNLayers(4);
  ch[nc].SetChannels(0,      0,     4,    8,    12);
  ch[nc].SetChannels(1,      3,     6,   11,    14);
  ch[nc].SetTracker(2);
  ch[nc].SetNames("dch_", "1y", "1y^", "2y", "2y^");
  ch[nc].SetGeometry(0.0, 2.6, 0.0, 4.2, 2.1); // Zy = 0.0
  // middle = -3.9 = -((4 - 1)*2.6)/2
  nc++;

  // module 0, socket 1 (cable 1x)
  ch[nc].SetNLayers(4);
  ch[nc].SetChannels(0,     16,    20,   24,   28);
  ch[nc].SetChannels(1,     19,    22,   27,   30);
  ch[nc].SetTracker(1);
  ch[nc].SetNames("dch_", "1x", "1x^", "2x", "2x^");
  ch[nc].SetGeometry(0.0, 2.6, 0.0, 4.2, 2.1); // Zx = 10.4
  // middle = -3.9 = -((4 - 1)*2.6)/2
  nc++;

  // module 0, socket 2 (trigger)
  ch[nc].SetNLayers(1);
  ch[nc].SetChannels(0,     46);
  ch[nc].SetChannels(1,     46);
  ch[nc].SetNames("", "trig");
  nc++;

  // module 0, socket 3 (hlaf cable 2y)
  ch[nc].SetNLayers(2);
  ch[nc].SetChannels(0,     48,    52);
  ch[nc].SetChannels(1,     51,    54);
  ch[nc].SetTracker(4);
  ch[nc].SetNames("dch_", "3y", "3y^");
  ch[nc].SetGeometry(0.0, 2.6, 0.0, 4.2, 2.1); // Zy = 88.8
  // middle = -29.7 - 1.3 = -29.7 - ((2 - 1)*2.6)/2
  nc++;

  // module 0, socket 3 (half cable 2y)
  ch[nc].SetNLayers(2);
  ch[nc].SetChannels(0,     56,    60);
  ch[nc].SetChannels(1,     59,    62);
  ch[nc].SetTracker(3);
  ch[nc].SetNames("dch_", "3x", "3x^");
  ch[nc].SetGeometry(0.0, 2.6, 0.0, 4.2, 2.1); // Zx = 94.0
  // middle = -29.7 - 1.3 = -29.7 - ((2 - 1)*2.6)/2
  nc++;

  // module 1, socket 0, 1 (not used)

  // module 1, socket 2 (cable 3y)
  ch[nc].SetNLayers(4);
  ch[nc].SetChannels(0,     96,  100,   104,   107);
  ch[nc].SetChannels(1,     99,  103,   106,   109);
  //  ch[nc].SetTracker(3); // common
  ch[nc].SetTracker(5); // single
  ch[nc].SetNames("dch_", "4x", "5x", "4x^", "5x^");
  ch[nc].SetGeometry(0.0, 2.6, 0.0, 4.2, 2.1); // Zx = 128.7
  // middle(common) = -29.7 + 39.9 - 6.5
  // middle(single) = -3.9 = -((4 - 1)*2.6)/2
  nc++;

  // module 1, socket 3 (half cable 3x)
  ch[nc].SetNLayers(2);
  ch[nc].SetChannels(0,    112,   116);
  ch[nc].SetChannels(1,    115,   118);
  ch[nc].SetTracker(4);
  ch[nc].SetNames("dch_", "6y", "6y^");
  ch[nc].SetGeometry(59.4, 2.6, 0.0, 4.2, 2.1); // Zy = 148.2
  // middle = 29.7 - 1.3 = 29.7 - ((2 - 1)*2.6)/2
  nc++;

  // module 1, socket 3 (half cable 3x)
  ch[nc].SetNLayers(2);
  ch[nc].SetChannels(0,    120,   124);
  ch[nc].SetChannels(1,    123,   126);
  ch[nc].SetTracker(3);
  ch[nc].SetNames("dch_", "6x", "6x^");
  ch[nc].SetGeometry(59.4, 2.6, 0.0, 4.2, 2.1); // Zx = 153.4
  // middle = 29.7 - 1.3 = 29.7 - ((2 - 1)*2.6)/2
  nc++;

  // module 2, socket 0, 1 (cable b1y, b1x)
  ch[nc].SetNLayers(4);
  // lower/right half
  ch[nc].SetChannels(0,    128, 132, 136, 140);
  ch[nc].SetChannels(1,    131, 134, 139, 142);
  // upper/left half
  ch[nc].SetChannels(0,    144, 147, 152, 155);
  ch[nc].SetChannels(1,    146, 149, 154, 157);
  ch[nc].SetTracker(10);
  ch[nc].SetNames("dch_", "10x", "10x^", "11x", "11x^");
  ch[nc].SetGeometry(0.0, 2.6, 0.0, 4.2, 2.1); // Zx = 10.4
  // middle = -3.9 = -((4 - 1)*2.6)/2
  nc++;

  // module 2, socket 2, 3 (cable "5", "red")
  ch[nc].SetNLayers(4);
  // lower/right half
  ch[nc].SetChannels(0,    160, 164, 168, 172);
  ch[nc].SetChannels(1,    163, 166, 171, 174);
  // upper/left half
  ch[nc].SetChannels(0,    176, 179, 184, 187);
  ch[nc].SetChannels(1,    178, 181, 186, 189);
  ch[nc].SetTracker(11);
  ch[nc].SetNames("dch_", "10y", "10y^", "11y", "11y^");
  ch[nc].SetGeometry(0.0, 2.6, 0.0, 4.2, 2.1); // Zy = 0.0
  // middle = -3.9 = -((4 - 1)*2.6)/2
  nc++;

  TString opt = option;
  if (opt.Contains("layers")) {
    if (opt.Contains("head")) {
      gSystem->Exec("more -99 sql/head_layers.sql");
      Printf("\n\n");
    }
    Printf("INSERT INTO `layers` (`Numb`, `Tracker`, `Name`, `NChan`, `Z`, `Shift`, `Delta`, `Range`, `Direct`) VALUES");
    for (Int_t ic = 0; ic < nc; ic++) {
      if (ic != (nc - 1)) {
	ch[ic].PrintLayers();
	Printf("");
      }
      else ch[ic].PrintLayers("last");
    }
  }

  if (opt.Contains("channels")) {
    if (opt.Contains("layers")) Printf("\n\n\n\n");
    if (opt.Contains("head")) {
      gSystem->Exec("more -99 sql/head_channels.sql");
      Printf("\n\n");
    }
    Printf("INSERT INTO `channels` (`Nadc`, `DetNumb`, `DetChan`, `T0`, `TMax`) VALUES");
    for (Int_t ic = 0; ic < nc; ic++) {
      if (ic != (nc - 1)) {
	ch[ic].PrintChannels();
	Printf("\n");
      }
      else ch[ic].PrintChannels("last");
    }
  }
}

// Author: Jan Musinsky
// 14/03/2016

/*
  .L macros/SimulateOnlineSpill.C+
  SimulateOnlineSpill("runXYZ.dat", 5)
 */

#include <TString.h>
#include <TDatime.h>

const char *sname = "/var/tmp/vme_recycle";

FILE *CreateSpillFile();
void RotateSpillFile(FILE *f, ULong_t first, UInt_t firstword, ULong_t last, UInt_t lastword);
void CheckSpill(UInt_t word);
void PrintTime(Bool_t empty = kFALSE);

//______________________________________________________________________________
void SimulateOnlineSpill(const char *fname, UInt_t delay = 5)
{
  FILE *dataFile = fopen(fname, "rb");
  if (!dataFile) {
    Printf("data file %s can not be opened", fname);
    return;
  }
  FILE *spillFile = CreateSpillFile();
  if (!spillFile) return;

  const size_t bufSize = 4096; // 4096 is blocksize in bytes (blockdev --getbsz /dev/sda1)
  UInt_t buffer[bufSize];      // data in buffer is 4 bytes (32 bits)
  size_t nread;                // number of read words

  UInt_t dataWord = 0;         // one word of data
  ULong_t nDataWords = 0;      // number of all words
  Bool_t nextSpill = kTRUE;    // next (new) spill
  UInt_t spillWord = 0;        // one word of spill
  ULong_t spillPosition = 0;   // position of next (new) spill

  do {
    nread = fread(buffer, sizeof(UInt_t), bufSize, dataFile);
    for (size_t i = 0; i < nread; i++) {
      dataWord = buffer[i];
      fwrite(&dataWord, sizeof(UInt_t), 1, spillFile);
      if (nextSpill) {
        nextSpill = kFALSE;
        spillWord = dataWord;
        spillPosition = nDataWords;
      }

      // spill trailer = 0xD (bits 31:28 = 1101)
      // spill trailer and end of spill data = 0x1B (bits 31:27 = 11011)
      if ((dataWord >> 27) == 0x1B) {
        RotateSpillFile(spillFile, spillPosition, spillWord, nDataWords, dataWord);
        spillFile = 0; // must be before break
        if (feof(dataFile) != 0) break;

        PrintTime();
        Printf("####### delay %d sec #######", delay);
        sleep(delay);
        nextSpill = kTRUE;
        spillFile = CreateSpillFile();
        if (!spillFile) return;
      }
      nDataWords++;
    }
  } while (nread == bufSize);

  if (spillFile) { // missed end of spill data
    nDataWords = nDataWords - 1;
    RotateSpillFile(spillFile, spillPosition, spillWord, nDataWords, dataWord);
  }
  fclose(dataFile);
}
//______________________________________________________________________________
FILE *CreateSpillFile()
{
  TString name(sname);
  name += ".dat.tmp";
  FILE *f = fopen(name.Data(), "wb");
  if (!f)
    Printf("spill file %s can not be created", name.Data());
  else {
    PrintTime();
    Printf("create file \t %s", name.Data());
  }
  return f;
}
//______________________________________________________________________________
void RotateSpillFile(FILE *f, ULong_t first, UInt_t firstword, ULong_t last, UInt_t lastword)
{
  if (!f) return;
  fclose(f);

  TString nameOld(sname);
  nameOld += ".dat.tmp";
  TString nameNew(sname);
  nameNew += ".dat";
  if (rename(nameOld.Data(), nameNew.Data()) != 0)
    Printf("file %s can not be renamed", nameOld.Data());
  else {
    PrintTime();
    Printf("rename file \t %s", nameNew.Data());
  }

  // numbers in word unit
  PrintTime(kTRUE);
  printf("word from    %12lu: [0x%08X]", first, firstword);
  CheckSpill(firstword);
  PrintTime(kTRUE);
  printf("word to      %12lu: [0x%08X]", last, lastword);
  CheckSpill(lastword);
  // word[0] (4bytes), word[1] (4bytes), word[2] (4bytes)
  // from word[0] to word[2] are total 3 words (not 2) = total 12 bytes
  last = last + 1;
  PrintTime(kTRUE);
  Printf("total words \t %lu", (last - first));

  // numbers in bytes unit
  first = first*sizeof(firstword);
  last = last*sizeof(firstword);
  PrintTime(kTRUE);
  Printf("bytes from \t %lu", first);
  PrintTime(kTRUE);
  Printf("bytes to \t %lu", last);
  PrintTime(kTRUE);
  Printf("total bytes \t %lu", last - first);
}
//______________________________________________________________________________
void CheckSpill(UInt_t word)
{
  if ((word >> 28) == 0xC) {                // spill header
    if ((word >> 27) == 0x18) printf("\n"); // spill header and normal data, OK
    else printf(" SHDR missed normal data\n");
  }
  else if ((word >> 28) == 0xD) {           // spill trailer
    if ((word >> 27) == 0x1B) printf("\n"); // spill trailer and end of spill data, OK
    else printf(" STRL missed end of spill data\n");
  }
  else
    printf(" !!! NO SPILL DATA !!!\n");
}
//______________________________________________________________________________
void PrintTime(Bool_t empty)
{
  if (empty) {
    printf("         ");
    return;
  }

  TDatime lt;
  printf("%02d:%02d:%02d ", lt.GetHour(), lt.GetMinute(), lt.GetSecond());
}

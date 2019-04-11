// 2019-04-11
// Jan Musinsky

#include <TSystem.h>
#include <TDatime.h>
#include <TRegexp.h>

class FileInfoStat : public TNamed {
public:
  Long64_t fSize;  // total size in bytes (from FileStat_t)
  Long_t   fMtime; // modification date   (from FileStat_t)
  Int_t    fRnum;  // digit run number

  FileInfoStat() : TNamed(), fSize(0), fMtime(0), fRnum(-1) { }
  FileInfoStat(const char *name, const char *title) : TNamed(name, title),
      fSize(0), fMtime(0), fRnum(-1) { }
  virtual ~FileInfoStat() { }
  virtual  Bool_t IsSortable() const { return kTRUE; }
  virtual  Int_t  Compare(const TObject *obj) const
  {
    const FileInfoStat *other = (FileInfoStat *)obj;
    if (!other) return 0;

    if (fSize < other->fSize) return -1;
    else if (fSize > other->fSize) return 1;
    else return 0;
  }
  virtual  void   Print(Option_t *option = "") const
  {
    TString opt = option; // file suffix
    opt.Remove(TString::kLeading, '.');
    TDatime dt;
    dt.Set(fMtime);
    Printf("%12lld %s run%03d.%s => %s", fSize, dt.AsSQLString(), fRnum, opt.Data(), GetName());
    // ls --time-style='+%Y-%m-%d %H:%M:%S' -ltr /mnt/free1/data_raw/2013_12/*.dat # -Gg
  }

  ClassDef(FileInfoStat, 0) // simple FileInfoStat
};
ClassImp(FileInfoStat)

Int_t todigit(TString digistring, Int_t ndigits)
{
  // find first consecutive ndigits in digistring and return digit as int
  // run_123vme => 123 (for 3 ndigits)
  // path/data_raw/2013_12 => 2013 (for 4 ndigits)

  TString sdigit;
  for (Int_t i = 0; i < ndigits; i++)
    sdigit += "[0-9]";
  TRegexp redigit(sdigit);
  Ssiz_t dpos = digistring.Index(redigit);
  if (dpos != TString::kNPOS) {
    digistring.Remove(0, dpos); // from begin to redigit
    digistring.Remove(ndigits); // from redigit to end
    return digistring.Atoi();
  } else
    return 0; // same as Atoi() from nondigit string
}

void DigitRunFiles()
{
  // find all regular *.fsuffix files in dirname and try idntify run number

  const char *dirname = "/mnt/free1/data_raw/2013_12";
  //const char *dirname = "/mnt/free1/data_raw/2013_12"; // trailing '/' is not important
  const char *fsuffix = ".dat";  // pattern ".fsuf" better than only "fsuf" (case sensitive)  !!! leading '.' is not important !!!!! alebo ako ROK popis
  const char *yearpat = "2013-"; // pattern "YYYY-" better than only "YYYY"

  //  root [] TString dirname = "/mnt/free1/data_raw/2013_12/"
  //  (TString &) "/mnt/free1/data_raw/2013_12/"[28]
  //  root [] dirname.Remove(TString::kTrailing, '/')
  //  (TString &) "/mnt/free1/data_raw/2013_12"[27]
  //  root [] todigit(dirname, 4)
  //  (int) 2013
  //  root []


  // TSystemFile or TSystemDirectory

  void *dir = gSystem->OpenDirectory(dirname);
  if (!dir) {
    Printf("no files in dir %s", dirname);
    return;
  }

  const char *filename = nullptr;
  FileInfoStat *fis = nullptr;
  TList *files = new TList();
  files->SetOwner(kTRUE);
  while ((filename = gSystem->GetDirEntry(dir))) {
    FileStat_t fs;
    if (gSystem->GetPathInfo(TString::Format("%s/%s", dirname, filename), fs) != 0) continue;
    if (R_ISDIR(fs.fMode)) continue;
    if (fs.fIsLink) continue; // don't use R_ISLNK(fs.fMode)
    TString fname = filename;
    if (!fname.EndsWith(fsuffix, TString::kExact)) continue;

    // remove file suffix
    fname.ReplaceAll(fsuffix, "");
    // remove datime string (if exist), YYYY-MM-DD_HH-MM-SS => 19 characters
    Ssiz_t pos = fname.Index(yearpat);
    if (pos != TString::kNPOS) fname.Remove(pos, 19);
    // find ndigits (run number)
    Int_t nrun = todigit(fname, 3);          // find NNN digits
    if (nrun == 0) nrun = todigit(fname, 2); // find NN digits
    if (nrun == 0) nrun = todigit(fname, 1); // find N digit

    fis = new FileInfoStat(filename, dirname);
    fis->fSize  = fs.fSize;
    fis->fMtime = fs.fMtime;
    fis->fRnum  = nrun;
    files->Add(fis);
  }
  gSystem->FreeDirectory(dir);

  files->Sort();
  files->Print(fsuffix);

  delete files;
}

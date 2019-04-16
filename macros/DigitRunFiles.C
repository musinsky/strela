// 2019-04-16
// Jan Musinsky

#include <TSystem.h>
#include <TPRegexp.h>
#include <TDatime.h>

#include <vector>

class FileInfoStat : public TNamed {
public:
  Long64_t fSize;  // total size in bytes (from FileStat_t)
  Long_t   fMtime; // modification date   (from FileStat_t)
  Int_t    fRun;   // digit run number

  FileInfoStat() : TNamed(), fSize(0), fMtime(0), fRun(0) { }
  FileInfoStat(const char *name, const char *title) : TNamed(name, title),
      fSize(0), fMtime(0), fRun(0) { }
  virtual ~FileInfoStat() { }
  virtual  Bool_t IsSortable() const { return kTRUE; }
  virtual  Int_t Compare(const TObject *obj) const
  {
    const FileInfoStat *other = dynamic_cast<const FileInfoStat *>(obj);
    if (!other) return 0;

    if (fMtime < other->fMtime) return -1;
    else if (fMtime > other->fMtime) return 1;
    else return 0;
  }

  ClassDef(FileInfoStat, 0) // simple FileInfoStat
};
ClassImp(FileInfoStat)

void DigitRunFiles(const char *dirname = nullptr, Bool_t lns = kFALSE)
{
  // find all *.fext regular files in dirname and try convert to digit run number

  if (!dirname) dirname = "/mnt/free1/data_raw/2013_12";
  const char *fext = ".dat"; // case sensitive (pattern ".fext" better than only "fext")
  TString dname = dirname;
  dname.Remove(TString::kTrailing, '/'); // only cosmetic
  void *dir = gSystem->OpenDirectory(dname.Data());
  if (!dir) {
    Printf("no files in dir \"%s\"", dname.Data());
    return;
  }

  // similar as TSystemFile or TSystemDirectory
  const char *filename = nullptr;
  FileInfoStat *fis = nullptr;
  TList *files = new TList();
  files->SetOwner(kTRUE);
  std::vector<Int_t> runs;
  while ((filename = gSystem->GetDirEntry(dir))) {
    FileStat_t fs;
    if (gSystem->GetPathInfo(TString::Format("%s/%s", dname.Data(), filename), fs) != 0) continue;
    if (R_ISDIR(fs.fMode)) continue;
    if (fs.fIsLink) continue; // don't use R_ISLNK(fs.fMode)
    TString fname = filename;
    if (!fname.EndsWith(fext, TString::kExact)) continue;

    // remove file extension
    fname.ReplaceAll(fext, "");

    // remove datime (PCRE perl compatible regular expression)
    // YYYY-MM-DD_HH-MM-SS (19 characters)
    // test YYYY from 1970 (start POSIX, UNIX Epoch time) to 2099
    // test HH from 00 to 23
    // https://regex101.com/r/mQT9HR/1
    TPRegexp re("(19[7-9]\\d{1}|20\\d{2})-(\\d{2})-(\\d{2})_([0-1]\\d{1}|2[0-3])-(\\d{2})-(\\d{2})");
    //    TObjArray *matches = re.MatchS(fname);
    //    for (Int_t i = 0; i < matches->GetEntries(); i++) {
    //      const TString match = ((TObjString *)matches->At(i))->GetString();
    //      Printf("%d] \"%s\"", i, match.Data());
    //    }
    //    delete matches; // owner TObjString objects
    Ssiz_t repos, relen;
    repos = fname.Index(re, &relen);
    if (repos != TString::kNPOS) fname.Remove(repos, relen);

    // try convert to digit run numbers (digit N, NN, NNN or NNNN)
    TPRegexp redigi("(\\d{1,4})");
    TString sdigi = fname(redigi); // convert TSubString to TString
    // fname(redigi).Data() != sdigi.Data() => TSubString vs. TString
    Int_t run = sdigi.Atoi(); // Atoi() for nondigit string = 0

    fis = new FileInfoStat(filename, dname.Data());
    fis->fSize  = fs.fSize;
    fis->fMtime = fs.fMtime;
    fis->fRun   = run;
    files->Add(fis);
    runs.push_back(run);
  }
  gSystem->FreeDirectory(dir);

  // print status
  TString bla = dname;
  bla.Remove(TString::kLeading, '/');
  bla.ReplaceAll("/", ".");
  FILE *fstatus = fopen(TString::Format("%s.status", bla.Data()), "w");
  FILE *fnolink = fopen(TString::Format("%s.nolink", bla.Data()), "w");
  if (!fstatus || !fnolink) {
    Printf("not writable dir \"%s\"", gSystem->pwd());
    delete files;
    return;
  }

  Printf("\ncreated \"%s\" (status and nolink) files", bla.Data());
  TDatime dt; // UNIX Epoch time (01.01.1995 => 31.12.2058) (datime as 32bit word)
  TString sfext = fext;
  sfext.Remove(TString::kLeading, '.');
  Int_t cnt = std::count_if(runs.begin(), runs.end(), [](Int_t r){return r > 0;});
  bla = TString::Format("# %s\n# %s:%s/", dt.AsSQLString(), gSystem->HostName(),
                        dname.Data());
  fprintf(fstatus, "%s\n# %d (*.%s) regular files were found => %d files were digitized\n\n",
          bla.Data(), files->GetEntries(), sfext.Data(), cnt);
  fprintf(fnolink, "%s\n# files without links (were not digitized or multiple)\n\n",
          bla.Data());

  files->Sort(kSortAscending); // sort by FileModifyDate
  for (Int_t i = 0; i < files->GetEntries(); i++) {
    fis = (FileInfoStat *)files->At(i);
    dt.Set(fis->fMtime); // UNIX Epoch time (TDatime origin 01.01.1995)
    TString digitfile = "";
    if ((fis->fRun) > 0) digitfile = TString::Format("run%03d.%s", fis->fRun, sfext.Data());
    TString status = "";
    cnt = std::count(runs.begin(), runs.end(), fis->fRun);
    if (cnt != 1) status = TString::Format("[%dx]", cnt);
    bla = TString::Format("%12lld %s %10s %4s => %s", fis->fSize, dt.AsSQLString(),
                          digitfile.Data(), status.Data(), fis->GetName());
    fprintf(fstatus, "%s\n", bla.Data());
    if (((fis->fRun) < 1) || (cnt != 1)) { // no link
      fprintf(fnolink, "%s   %s/%s\n", bla.Data(), fis->GetTitle(), fis->GetName());
    } else if (lns) { // make symbolic links
      // don't use "--force" (any dirname ~ same digitfile)
      gSystem->Exec(TString::Format("ln -s %s/%s %s", fis->GetTitle(), fis->GetName(),
                                    digitfile.Data()));
    }
  }

  cnt = std::count_if(runs.begin(), runs.end(), [](Int_t r){return r > 999;});
  if (cnt) Printf("%d NNNN (4 digit) files", cnt);

  delete files; // owner FileInfoStat objects
  fclose(fstatus);
  fclose(fnolink);
}

// cd /mnt/free1/data_raw/2013_12
// ls --time-style='+%Y-%m-%d %H:%M:%S' -ltr /mnt/free1/data_raw/2013_12/*.dat # -Gg
// sed -i '1 i\\' $HOME/mmm

// 2020-06-25
// Jan Musinsky

/*
 * root[] .L DatimeRFiles.C
 * root[] DatimeRFiles("c1.root")
 * root[] DatimeRFiles("/mnt/free1/data/2003_06/", kTRUE)
 */

#include <TSystem.h>
#include <TFile.h>

void CompareModTime(const char *fn, Long_t mtime, Bool_t change);

void DatimeRFiles(const char *dfname = "none", Bool_t touch = kFALSE)
{
  // dfname is dir name with root files or root file name
  // compare datime of root modification with unix modtime of file(s)

  const char *ext = ".root";
  Bool_t onefile = kFALSE;
  TString dname = dfname;

  // one dfname root file
  if (dname.EndsWith(ext, TString::kExact)) {
    onefile = kTRUE;
    dname = gSystem->GetWorkingDirectory(); // pwd
  }

  // all root files in dfname dir
  dname.Remove(TString::kTrailing, '/'); // only cosmetic (concat//path = concat/path)
  void *dir = gSystem->OpenDirectory(dname.Data());
  if (!dir) {
    Printf("cannot open dir or non root file \"%s\"", dfname);
    return;
  }

  const char *filename = nullptr;
  while ((filename = gSystem->GetDirEntry(dir))) {
    if (onefile) filename = dfname;
    FileStat_t fs;
    TString fname = dname + "/" + filename;
    if (gSystem->GetPathInfo(fname.Data(), fs) != 0) {
      if (onefile) {
        Printf("root file does not exist \"%s\"", dfname);
        break;
      }
      continue;
    }
    if (R_ISDIR(fs.fMode)) continue;
    if (fs.fIsLink) continue; // don't use R_ISLNK(fs.fMode)
    if (!fname.EndsWith(ext, TString::kExact)) continue;

    CompareModTime(fname.Data(), fs.fMtime, touch);
    if (onefile) break; // don't use dir = nullptr (need FreeDirectory)
  }
  gSystem->FreeDirectory(dir);
}

void CompareModTime(const char *fn, Long_t mtime, Bool_t change = kFALSE)
{
  TFile *file = TFile::Open(fn, "READ");
  if (!file) {
    Printf("cannot open root file \"%s\"", fn);
    return;
  }

  // UNIX Epoch time (TDatime from 1995 to 2058, TDatime origin 01.01.1995)
  const TDatime root_create = file->GetCreationDate();
  const TDatime root_modify = file->GetModificationDate();
  const TDatime unix_modify(mtime);
  file->Close(); // close before "touch"

  Int_t del_root   = root_modify.Get() - root_create.Get();
  Int_t del_modify = unix_modify.Get() - root_modify.Get();
  TString sroot_create = root_create.AsSQLString();
  TString sroot_modify = root_modify.AsSQLString();
  TString sunix_modify = unix_modify.AsSQLString();
  // !!! WARNING !!! printf does not work correctly with ?!circular?! buffer
  // don't use Printf("%s %s", root_modify.AsSQLString(), unix_modify.AsSQLString())

  Printf("file \"%s\"", file->GetName());
  Printf(" root_create: %s\n root_modify: %s   => root_modify-root_create: %d",
         sroot_create.Data(), sroot_modify.Data(), del_root);
  Printf(" unix_modify: %s   => unix_modify-root_modify: %d",
         sunix_modify.Data(), del_modify);
  if (!change) return;

  // similar as TDataSetManagerFile::CheckLocalCache() or TMD5::FileChecksum()
  UInt_t troot = root_modify.Convert();
  if (gSystem->Utime(fn, troot, 0) != 0) // actime = modtime
    Printf("cannot set modification datime on file \"%s\" (errno: %d)",
           fn, TSystem::GetErrno());
  else
    Printf("set root_modify datime on file \"%s\"", fn);
}

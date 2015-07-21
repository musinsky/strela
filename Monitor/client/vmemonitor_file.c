// Musinsky Jan
// 2015-07-21

#include "vmemonitor_file.h"
#include "vmemonitor.h"

FILE *openFile(const char *dname, const char *fname)
{
  if ((dname == NULL) || (fname == NULL)) return NULL;

  char fullname[PATH_MAX];
  FILE *fstream;

  // prefer strcat to sprintf
  strcpy(fullname, dname);
  strcat(fullname, "/");
  strcat(fullname, fname);

  fstream = fopen(fullname, "rb");
  if (fstream == NULL) perror("fopen failed");

  if (fstream && vmode) {
    printTime();
    fprintf(stderr, "open \t\t %s\n", fullname);
  }

  return fstream;
}

void closeFile(FILE *fstream)
{
  if (fstream == NULL) return;
  if (fclose(fstream) != 0) perror("fclose failed");

  if (vmode) {
    printTime();
    fprintf(stderr, "close file\n");
  }
}

long findLastType(FILE *fstream, vmedata_type datatype, long ntimes)
{
  // backwards search in fstream (VME data) for ntimes the datatype
  // return offset in bytes relative to begin (SEEK_SET) of the fstream

  if (fstream == NULL) {
    fprintf(stderr, "no fstream\n");
    return -1;
  }

  size_t nread, iw, wordsize, bufsize;
  long filesize, blockpos, ndatawords, ndatatypes;

  wordsize = sizeof(uint32_t); // minimal word size of VME data is 4 bytes (32 bits)
  uint32_t dataword;           // one word of VME data
  uint32_t buf[BLOCK_SIZE];    // buffer of 4*BLOCK_SIZE = 16384 bytes size

  if (fseek(fstream, 0, SEEK_END) != 0) {
    perror("fseek failed");
    return -1;
  }

  filesize = ftell(fstream);
  if (filesize == -1) {
    perror("ftell failed");
    return -1;
  }
  if ((filesize % wordsize) != 0) { // specificity of VME data
    fprintf(stderr, "size of VME file not divisible by %lu\n", wordsize);
    return -1;
  }

  blockpos = filesize; // end of the file
  bufsize = BLOCK_SIZE;
  ndatawords = 0;
  ndatatypes = 0;

  do {
    if (fseek(fstream, -(ndatawords + BLOCK_SIZE)*wordsize, SEEK_END) != 0) {
      // fseek (after error) did not change current position
      bufsize = blockpos/wordsize;
      if (bufsize >= BLOCK_SIZE) {
        fprintf(stderr, "something wrong in findLastType\n");
        return -1;
      }
      fseek(fstream, 0, SEEK_SET); // rewind to beginning of the file
    }

    blockpos = ftell(fstream);
    if (blockpos == -1) {
      perror("ftell (blockpos) failed");
      return -1;
    }

    nread = fread(buf, wordsize, bufsize, fstream);
    for (iw = (nread-1); iw < nread; --iw) { // size_t {aka long unsigned int}
      dataword = buf[iw];
      ndatawords++;
      //      printf("%12lu: [0x%08X]\n", filesize/wordsize - ndatawords, dataword);
      if ((dataword >> 28) == datatype) {
        // skip end of spill data
        if ((datatype == VME_SHDR) && ((dataword >> 27) == 0x19)) continue;
        ndatatypes++;
        if (ndatatypes == ntimes) return (filesize - ndatawords*wordsize); // in bytes
      }
    }
  } while (nread == BLOCK_SIZE);

  return -1;
}

int endsWith(const char *name, const char *ext)
{
  // if name string ends with ext string return length of name string
  // otherwise return -1 (similar as TString::EndsWith() from ROOT)

  if (name == NULL) return -1;
  size_t namel = strlen(name);
  if (ext == NULL) return namel;
  size_t extl = strlen(ext);

  if (extl > namel) return -1;
  if (strncmp(name + namel - extl, ext, extl) == 0) return namel;

  return -1;
}

int anotherName(const char *name)
{
  static char current[NAME_MAX + 1]; // 256, must be static

  if (name == NULL) return -1;
  if (strlen(name) > NAME_MAX) {
    fprintf(stderr, "max. length of name = %d\n", NAME_MAX);
    return -1;
  }

  if (strcmp(current, name) != 0) { // another
    strcpy(current, name);
    return 0;
  }

  return -1;
}

// Musinsky Jan
// 2015-06-24

#ifndef VMEMONITOR_FILE_H
#define VMEMONITOR_FILE_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#define BLOCK_SIZE 4096 // blocksize in bytes (blockdev --getbsz /dev/sda1)

typedef enum {
  VME_EHDR = 0xA, // Event header
  VME_SHDR = 0xC  // Spill header
} vmedata_type;

FILE *openFile(const char *dname, const char *fname);

void closeFile(FILE *fstream);

long findLastType(FILE *fstream, vmedata_type datatype, long ntimes);

int endsWith(const char *name, const char *ext);

int anotherName(const char *name);

#endif

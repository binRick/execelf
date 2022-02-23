#include "log.c"
#include "microtar.c"
#include "microtar.h"
#include <stdbool.h>
#include <stdio.h>

#define WRITE_TAR_FILE1    ".test-write-1.tar"
#define READ_TAR_FILE1     ".test-write-1.tar"
#define READ_TAR_FILE2     ".binary-files1.tar"
#define FIND_FILE1         "test2.txt"
#define FIND_FILE2         "bin/pexec.static"
#define FIND_FILE2a        "bin/client.static"

mtar_t        tar;
mtar_header_t h;

const char    *str1 = "Hello world";
const char    *str2 = "Goodbye worxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxld";

#define DO_WRITE     true
#define DO_READ1     true
#define DO_READ2     true
#define DO_FIND1     true
#define DO_FIND2     true
#define DO_FIND2a    true


int main(){
  char *p;

  if (DO_WRITE) {
    mtar_open(&tar, WRITE_TAR_FILE1, "w");
    mtar_write_file_header(&tar, "test1.txt", strlen(str1));
    mtar_write_data(&tar, str1, strlen(str1));
    mtar_write_file_header(&tar, "test2.txt", strlen(str2));
    mtar_write_data(&tar, str2, strlen(str2));
    mtar_finalize(&tar);
    mtar_close(&tar);
  }

  if (DO_READ1) {
    mtar_open(&tar, READ_TAR_FILE1, "r");

    while ((mtar_read_header(&tar, &h)) != MTAR_ENULLRECORD) {
      printf("%s>    -    %s (%d bytes)\n", READ_TAR_FILE1, h.name, h.size);
      mtar_next(&tar);
    }
    if (DO_FIND1) {
      mtar_find(&tar, FIND_FILE1, &h);
      p = calloc(1, h.size + 1);
      mtar_read_data(&tar, p, h.size);
      printf("      %s>  +  file size: %dB", FIND_FILE1, h.size);
      free(p);
    }
    mtar_close(&tar);
  }

  if (DO_READ2) {
    mtar_open(&tar, READ_TAR_FILE2, "r");

    while ((mtar_read_header(&tar, &h)) != MTAR_ENULLRECORD) {
      printf("   ** %s>  +  %s (%d bytes)\n", READ_TAR_FILE2, h.name, h.size);
      mtar_next(&tar);
    }
    if (DO_FIND2) {
      mtar_find(&tar, FIND_FILE2, &h);
      p = calloc(1, h.size + 1);
      mtar_read_data(&tar, p, h.size);
      printf(" ------  %s> file size: %dB\n", FIND_FILE2, h.size);
      free(p);
    }
    if (DO_FIND2a) {
      mtar_find(&tar, FIND_FILE2a, &h);
      p = calloc(1, h.size + 1);
      mtar_read_data(&tar, p, h.size);
      printf(" ------  %s> file size: %dB\n", FIND_FILE2a, h.size);
      free(p);
    }
    mtar_close(&tar);
  }


  return(0);
} /* main */

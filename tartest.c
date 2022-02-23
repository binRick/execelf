#include "log.c"
#include "microtar.c"
#include "microtar.h"
#include <stdio.h>

#define WRITE_TAR_FILE1    ".test-write-1.tar"
#define READ_TAR_FILE1     ".test-read-1.tar"
#define FIND_FILE11        "bin/client"

mtar_t        tar;
mtar_header_t h;

const char    *str1 = "Hello world";
const char    *str2 = "Goodbye worxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxld";


int main(){
  char *p;


  mtar_open(&tar, WRITE_TAR_FILE1, "w");
  mtar_write_file_header(&tar, "test1.txt", strlen(str1));
  mtar_write_data(&tar, str1, strlen(str1));
  mtar_write_file_header(&tar, "test2.txt", strlen(str2));
  mtar_write_data(&tar, str2, strlen(str2));
  mtar_finalize(&tar);
  mtar_close(&tar);

  mtar_open(&tar, READ_TAR_FILE1, "r");

  while ((mtar_read_header(&tar, &h)) != MTAR_ENULLRECORD) {
    printf("%s (%d bytes)\n", h.name, h.size);
    mtar_next(&tar);
  }

  mtar_find(&tar, FIND_FILE11, &h);
  p = calloc(1, h.size + 1);
  mtar_read_data(&tar, p, h.size);
  printf("file size: %dB", h.size);
  free(p);

  mtar_close(&tar);
  return(0);
}

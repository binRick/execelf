#include "log.c"
#include "microtar.c"
#include "microtar.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/memfd.h>
#include <stdbool.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

#define READ_TAR_FILE    ".binary-files1.tar"
#define LAUNCHER_FILE    "bin/pexec.static"
#define EXEC_FILE        "bin/client"

mtar_t        tar;
mtar_header_t h;


extern char **environ;


ssize_t cksys(const char *msg, ssize_t r) {
  if (r >= 0) {
    return(r);
  }
  fprintf(stderr, "Fatal Error in %s: %s\n", msg, strerror(errno));
  _exit(1);
}


void safe_ftruncate(int fd, off_t len) {
  while (true) {
    int r = ftruncate(fd, len);
    if (r == -1 && errno == EINTR) {
      continue;
    }
    cksys("ftruncate()", r);
    return;
  }
}


void transfer_mmap(int fdin, int fdout) {
  size_t off = 0, avail = 1024 * 1024 * 2; // 2MB

  // Allocate space in the memfd and map it into our userspace memory
  safe_ftruncate(fdout, avail); // We know fdout is a memfd
  char *buf = (char *)mmap(NULL, avail, PROT_WRITE, MAP_SHARED, fdout, 0);

  cksys("mmap()", (ssize_t)buf);

  while (true) {
    // We ran out of space - double the size of the buffer and
    // remap it into memory
    if (off == avail) {
      const size_t nu = avail * 2;
      safe_ftruncate(fdout, nu); // We know fdout is a memfd
      buf = mremap(buf, avail, nu, MREMAP_MAYMOVE);
      cksys("mremap()", (ssize_t)buf);
      avail = nu;
    }

    // Write data directly to the mapped buffer
    ssize_t r = read(fdin, buf + off, avail - off);
    if (r == 0) {
      break;
    }
    if (r == -1 && errno == EINTR) {
      continue;
    }
    cksys("read()", r);
    off += r;
  }

  // Truncate to final size
  safe_ftruncate(fdout, off);
  // munmap – no need; fexecve unmaps automatically
}


// Transfer data from one fd into the other using splice
// Returns 0 if the data was transferred successfully, -1
// if the underlying file type is not supported and exits
// on any other error.
int transfer_splice(int fdin, int fdout) {
  for (size_t cnt = 0; true; cnt++) {
    // Transferring 2GB at a time; this should be portable for 32bit
    // systems (and linux complains at the max val for uint64_t)
    ssize_t r = splice(fdin, NULL, fdout, NULL, ((size_t)1) << 31, 0);
    if (r == 0) {
      return(0);          // We're done
    }
    if (r < 0 && errno == EINTR) {
      continue;
    }
    if (r < 0 && errno == EINVAL && cnt == 0) {
      return(-1);                                        // File not supported
    }
    cksys("splice()", r);
  }
}


int main(int argc __attribute__((unused)), char *argv[]) {
int  TS_FD = 0;
if(argc>2){
  get_tar_file(argv[1], argv[2], 1);
  char msg[256];
  exit(0);
}
  const ssize_t f = cksys("memfd_create()", syscall(SYS_memfd_create, "Virtual File", MFD_CLOEXEC));

  if (transfer_splice(TS_FD, f) < 0) {
    transfer_mmap(TS_FD, f);
  }

  cksys("fexecve()", fexecve(f, argv, environ));
  fprintf(stderr, "Fatal Error in fexecve(): Should have terminated the process");

  return(1);
}


int get_tar_file(char *tf, char *ef, int *fd){
  char *p;

  mtar_open(&tar, tf, "r");
  while ((mtar_read_header(&tar, &h)) != MTAR_ENULLRECORD) {
//    fprintf(stderr, "   ** %s>  +  %s (%d bytes)\n", tf, h.name, h.size);
    mtar_next(&tar);
  }
  mtar_find(&tar, ef, &h);
  p = calloc(1, h.size + 1);
  mtar_read_data(&tar, p, h.size);
 // fprintf(stderr, " ------  %s> file size: %dB\n", ef, h.size);
//seek( fd, 0, SEEK_SET );
  write(fd, p, h.size);
  //close(fd);
//memcpy(&buf, p, h.size);
//return tar.stream;
// free(p);
}


/*
 * void wtf(){
 * FILE *stream;
 * char list[30];
 * int  i, numread, numwritten;
 *
 * // Open file in text mode:
 * if( fopen_s( &stream, "fread.out", "w+t" ) == 0 )
 * {
 *    for ( i = 0; i < 25; i++ )
 *       list[i] = (char)('z' - i);
 *    // Write 25 characters to stream
 *    numwritten = fwrite( list, sizeof( char ), 25, stream );
 *    printf( "Wrote %d items\n", numwritten );
 *    fclose( stream );
 *
 * }
 * else
 *    printf( "Problem opening the file\n" );
 *
 * if( fopen_s( &stream, "fread.out", "r+t" ) == 0 )
 * {
 *    // Attempt to read in 25 characters
 *    numread = fread( list, sizeof( char ), 25, stream );
 *    printf( "Number of items read = %d\n", numread );
 *    printf( "Contents of buffer = %.25s\n", list );
 *    fclose( stream );
 * }
 * else
 *    printf( "File could not be opened\n" );
 * }
 */


int tar_main(char *f){
  char *p;

  mtar_open(&tar, READ_TAR_FILE, "r");
  while ((mtar_read_header(&tar, &h)) != MTAR_ENULLRECORD) {
    fprintf(stderr, "   ** %s>  +  %s (%d bytes)\n", READ_TAR_FILE, h.name, h.size);
    mtar_next(&tar);
  }
  mtar_find(&tar, LAUNCHER_FILE, &h);
  p = calloc(1, h.size + 1);
  mtar_read_data(&tar, p, h.size);
  fprintf(stderr, " ------  %s> file size: %dB\n", LAUNCHER_FILE, h.size);
  free(p);
  mtar_find(&tar, EXEC_FILE, &h);
  p = calloc(1, h.size + 1);
  mtar_read_data(&tar, p, h.size);
  fprintf(stderr, " ------  %s> file size: %dB\n", EXEC_FILE, h.size);
  free(p);
  mtar_close(&tar);
  return(0);
} /* main */

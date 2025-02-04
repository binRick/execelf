#define _GNU_SOURCE
#define MAX_ARGS    256
#include "execelf.h"


int file_check(char *filename) {
  FILE *executable = fopen(filename, "r");

  if (executable == NULL) {
    fprintf(stderr, "Error: File '%s' was not found!\n", filename);
    return(1);
  }
  fclose(executable);
  return(0);
}


static void file_execution(size_t size, char *elf, char **f_file, char **envp) {
  int des = memfd_create("targ_file", FD_CLOEXEC);

  write(des, elf, size);
  sprintf(e_elf, "/proc/self/fd/%u", des);
  execve(e_elf, f_file, envp);
}


int main(int argc, char **argv, char **envp) {
  int  des;
  char *elf;

  if (argc == 1) {
    fprintf(stderr, "Usage: %s <elf_file>\n", argv[0]);
    exit(EXIT_FAILURE);
  } else {
    if (file_check(argv[1]) == 1) {
      exit(EXIT_FAILURE);
    } else {
      des = open(argv[1], O_RDONLY);
      fstat(des, &l_stat);
      elf = malloc(l_stat.st_size);
      read(des, elf, l_stat.st_size);
      char *args[MAX_ARGS] = {
        argv[1],
      };
      for (int i = 2; i < argc; ++i) {
        args[i - 2] = argv[i];
      }
      args[argc] = NULL;
      file_execution(l_stat.st_size, elf, &argv[1], envp);
      file_execution(l_stat.st_size, elf, args, envp);
    }
  }
  EXIT_SUCCESS;
}

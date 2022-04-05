#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/common.h"

int main(int argc, char *argv[]) {
  printf("hello world (pid: %d) \n", (int)getpid());
  // Se crea el proceso hijo, que es una copia casi exacta del padre.
  int rc = fork();
  if (rc < 0) {
    fprintf(stderr, "fork failed \n");
    exit(1);
  } else if (rc == 0) {
    // proceso hijo, el valor de  rc(pid) es cero.
    printf("Hello, i'm child (pid: %d)\n", (int)getpid());
  } else {
    // proceso padre, el valor rc es el pid del hijo.
    printf("Hello, i'm parent of %d (pid: %d)\n", rc, (int)getpid());
  }
  return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("hello, i'm parent (pid: %d)\n", getpid());
  // Acá se crea el hijo, el valor devuelto al hijo es distinto al del padre.
  int rc = fork();
  if (rc < 0) {
    fprintf(stderr, "fork failed \n");
  } else if (rc == 0) {
    // al hijo fork() le devuelve 0.
    printf("hello, i'm child (pid: %d)\n", getpid());
  } else {
    // al padre fork() le devuelve el pid del hijo.
    // wait() vuelve una vez que el hijo haya terminado.
    int rc_wait = wait(NULL);
    printf("hello, i'm parent of %d (rc_wait: %d) (pid: %d)\n", rc, rc_wait,
           getpid());
  }
  return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid;

  pid = fork();

  if (pid == 0) { /* Hijo */
    printf(" Soy  Luke(hijo)   ....    mi pid es : %d\n", getpid());
    exit(0);
  }

  /* Padre */

  int status;
  pid_t p = wait(&status); // El padre(proceso actual) no seguirá ejecutandose
                           // hasta que termine el hijo.

  printf(" Mi pid es %d .... Luke soy tu padre!!!!\n", getpid());
  return 0;
}

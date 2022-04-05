#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pid_t Fork() {
  pid_t pid;

  pid = fork();
  if (pid < 0) { /* error */
    perror(" Error de fork");
    exit(-1);
  }
  return pid;
}

int main() {
  Fork(); // El actual crea 1er hijo. Este hijo crea otro en la sig. línea.
  Fork(); // El actual crea 2do hijo.
  printf("Hola! padre : %d -> mi PID: %d\n", getppid(), getpid());
  exit(0);
}

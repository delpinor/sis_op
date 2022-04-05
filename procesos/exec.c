#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
  printf("hello world (pid:%d)\n", (int)getpid());
  int rc = fork();
  if (rc < 0) {

    // fork failed; exit
    fprintf(stderr, "fork failed\n");
    exit(1);

  } else if (rc == 0) {

    // child (new process)
    printf("hello, I am child (pid:%d)\n", (int)getpid());
    char *myArgs[3];
    myArgs[0] = strdup("wc");     // programa wc
    myArgs[1] = strdup("exec.c"); // arg: programa a ser contado
    myArgs[2] = NULL;             // marca el fin del arreglo
    execvp(myArgs[0], myArgs);
    printf("Esto no deberia imprimirse pues es totalmente reemplazado");

  } else {

    // parent goes down this path (main)
    int wc = wait(NULL);
    printf("hello, I am parent of %d (wc:%d) (pid:%d)\n", rc, wc,
           (int)getpid());
  }
}

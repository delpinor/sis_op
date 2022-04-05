#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int rc = fork();
  if (rc < 0) {
    fprintf(stderr, "fork failed \n");
  } else if (rc == 0) {
    printf("I'm child (pid: %d) \n", getpid());
    char *myargs[3];
    myargs[0] = "wc";
    myargs[1] = "p3.c";
    myargs[2] = NULL;
    execvp(myargs[0], myargs);
    printf("this should not print out \n");
  } else {
    int rc_wait = wait(NULL);
    printf("I'm parent of %d (rc_wait: %d) (pid: %d)\n", rc, rc_wait, getpid());
  }
}
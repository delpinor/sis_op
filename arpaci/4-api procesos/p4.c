#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>  // Para S_IRWXU
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int rc = fork();
  if (rc < 0) {
    fprintf(stderr, "fork failed \n");
  } else if (rc == 0) {
    // Cierro la salida estándar(pantalla)
    close(STDOUT_FILENO);
    // Direcciono la salida la archivo.
    open("salida.txt", O_CREAT | O_WRONLY | O_TRUNC | S_IRWXU);
    char *myargs[3];
    myargs[0] = "wc";
    myargs[1] = "p3.c";
    myargs[2] = NULL;
    execvp(myargs[0], myargs);
    printf("this should not print out \n");
  } else {
    int rc_wait = wait(NULL);
  }
}
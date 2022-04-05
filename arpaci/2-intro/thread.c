#include <stdio.h>
#include <stdlib.h>

#include "../include/common.h"
#include "../include/common_threads.h"

/*
volatile:

Hay al menos tres razones comunes para usarlo, todas relacionadas con
situaciones en las que el valor de la variable puede cambiar sin que el código
visible actúe:

1. Cuando interactúa con hardware que puede cambiar el valor.
2. Cuando hay otro hilo en ejecución que también usa la variable.
3. cuando hay un manejador de señales que puede cambiar el valor de la variable.

*/
volatile int counter = 0;
int loops;

void *worker(void *arg) {
  int i;
  for (i = 0; i < loops; i++) {
    counter++;
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: threads <value>\n");
    exit(1);
  }
  loops = atoi(argv[1]);  // converts string to int
  pthread_t p1, p2;
  printf("Initia value: %d\n", counter);

  pthread_create(&p1, NULL, worker, NULL);
  pthread_create(&p2, NULL, worker, NULL);
  pthread_join(p1, NULL);
  pthread_join(p2, NULL);

  printf("Final value: %d\n", counter);
  return 0;
}
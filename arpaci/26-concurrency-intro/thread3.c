#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
  int a;
  int b;
} myarg_t;

typedef struct {
  int x;
  int y;
} myret_t;

void *mythread(void *arg) {
  myret_t rvals;  // Se crea en el Stack: muy malo
  rvals.x = 1;
  rvals.y = 2;
  return (void *)&rvals;
}

int main(int argc, char *argv[]) {
  pthread_t thread;
  myret_t *rvals;
  myarg_t args = {12, 14};
  pthread_create(&thread, NULL, mythread, &args);
  pthread_join(thread, (void **)&rvals);
  printf("Value returned: %d, %d\n", rvals->x, rvals->y);
  free(rvals);
  return 0;
}
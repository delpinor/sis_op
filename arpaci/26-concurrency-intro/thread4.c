#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/common.h"

static volatile int counter = 0;
pthread_mutex_t lock;
// Otra opción, se evita la función pthread_mutex_init:
// pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *mythread(void *arg) {
  printf("%s: begin\n", (char *)arg);
  for (int i = 0; i < 1e7; i++) {  // 1 million
    pthread_mutex_lock(&lock);     // para evitar Race Condition
    counter = counter + 1;
    pthread_mutex_unlock(&lock);
  }
  printf("%s: end\n", (char *)arg);
  return NULL;
}

int main(int argc, char *argv[]) {
  int rc = pthread_mutex_init(&lock, NULL);
  assert(rc == 0);
  printf("main: begin\n");
  pthread_t thread1, thread2;
  pthread_create(&thread1, NULL, mythread, "A");
  pthread_create(&thread2, NULL, mythread, "B");
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  printf("counter value: %d\n", counter);
  printf("main: end\n");
  pthread_mutex_destroy(&lock);
  return 0;
}
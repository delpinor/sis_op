#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/common.h"

static volatile int counter = 0;

void *mythread(void *arg) {
  printf("%s: begin\n", (char *)arg);
  for (int i = 0; i < 1e7; i++) {  // 1 million
    counter = counter + 1;
  }
  printf("%s: end\n", (char *)arg);
  return NULL;
}

int main(int argc, char *argv[]) {
  printf("main: begin\n");
  pthread_t thread1, thread2;
  pthread_create(&thread1, NULL, mythread, "A");
  pthread_create(&thread2, NULL, mythread, "B");
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  printf("counter value: %d\n", counter);
  printf("main: end\n");
  return 0;
}
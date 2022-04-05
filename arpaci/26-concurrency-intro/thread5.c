#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/common.h"

// Inicializo la variable global
static volatile int ready = 0;

// Inicializo el mutex y el cond
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *hilo1(void *arg) {
  // Antes del wait debe tener bloqueado el mutex
  pthread_mutex_lock(&lock);
  // Le paso el lock, porque además de dormir el hilo, libera el lock. Así
  // cuando vuelve del wait, recupera el lock.
  while (ready == 0) {
    pthread_cond_wait(&cond, &lock);
  }
  pthread_mutex_unlock(&lock);
  printf("Hilo 1: fin\n");
  return NULL;
}

void *hilo2(void *arg) {
  // Antes del wait debe tener bloqueado el mutex
  pthread_mutex_lock(&lock);
  ready = 1;
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&lock);
  printf("Hilo 2: fin\n");
  return NULL;
}

int main(int argc, char *argv[]) {
  pthread_t thread1, thread2;
  pthread_create(&thread1, NULL, hilo1, NULL);
  pthread_create(&thread2, NULL, hilo2, NULL);
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  pthread_mutex_destroy(&lock);
  return 0;
}
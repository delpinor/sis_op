#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pthread_mutex_t tmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t tcond = PTHREAD_COND_INITIALIZER;
void *contar() {
  printf("Incio hijo\n");
  pthread_mutex_lock(&tmutex);
  for (int i = 0; i < 0xFFFFFFFF; i++)
    ;
  // Envio la señal(despertar) al padre(en este caso) una vez
  // terminado el ciclo
  pthread_cond_signal(&tcond);
  pthread_mutex_unlock(&tmutex);
  printf("Fin hijo\n");
  return NULL;
}
void esperarAlHijo() {
  pthread_mutex_lock(&tmutex);
  // Libero el lock y pongo en espera/bloqueado al padre.
  pthread_cond_wait(&tcond, &tmutex);
  pthread_mutex_unlock(&tmutex);
}

int main(int argc, char const *argv[]) {
  pthread_t p;
  printf("Inicio padre\n");
  pthread_create(&p, NULL, contar, NULL);
  esperarAlHijo();
  printf("Fin del padre\n");
  return 0;
}

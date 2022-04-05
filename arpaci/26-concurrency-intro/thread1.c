#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

/*
Usando typedef:


typedef struct {
  int age;
  char *name;
} Person;

Se declara:

Person unapersona;

*/
struct Person {
  int age;
  char *name;
};

void *mythread(void *arg) {
  struct Person *person = (struct Person *)arg;
  printf("Name: %s, age: %d\n", person->name, person->age);
  return NULL;
}

int main(int argc, char *argv[]) {
  printf("main: begin\n");
  pthread_t thread;
  struct Person person = {34, "Gonzalo"};
  pthread_create(&thread, NULL, mythread, &person);
  pthread_join(thread, NULL);
  printf("main: end\n");
  return 0;
}
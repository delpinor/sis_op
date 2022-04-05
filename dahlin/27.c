#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int staticVar = 0;
int main(int argc, char *argv[]) {
  staticVar += 1;
  sleep(10);
  printf("Address: %p Value: %d\n", &staticVar, staticVar);
}
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct node {
  int data;
  struct node *next;
};
typedef struct node node;
struct queue {
  int count;
  node *front;
  node *rear;
};
typedef struct queue queue;
void crear(queue *q);
void encolar(queue *q, int value) {
  node *tmp;
  tmp = malloc(sizeof(node));
  tmp->data = value;
  tmp->next = NULL;
  if (!isempty(q)) {
    q->rear->next = tmp;
    q->rear = tmp;
  } else {
    q->front = q->rear = tmp;
  }
  q->count++;
}
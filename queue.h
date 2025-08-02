#ifndef QUEUE_H_
#define QUEUE_H_

struct Node {
  char* data;
  struct Node* next;
};

typedef struct Queue {
  struct Node* head;
  struct Node* tail;
}  Queue;

int isEmpty(Queue*);
void enqueue(Queue*, const char*, int);
char* peek(Queue*, int*);
int dequeue(Queue*, char*, int*);

#endif // QUEUE_H_

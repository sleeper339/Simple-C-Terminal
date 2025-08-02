#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "queue.h"

int isEmpty(Queue* queue) {
  return queue->head == NULL;
}

void enqueue(Queue* queue, const char* source, int length) {
  char* buffer = malloc(length+1);
  struct Node* newNode = malloc(sizeof(struct Node));
  strncpy(buffer, source, length+1);
  buffer[length] = '\0';
  newNode->data = buffer;
  newNode->next = NULL;

  if(queue->head == NULL) {
    queue->head = queue->tail = newNode;
  } else {
    queue->tail->next = newNode;
    queue->tail = newNode;
  }
}

int dequeue(Queue* queue, char* buffer, int* length) {
  if(isEmpty(queue)) {return -1;} //Cannot dequeue an empty queue
  unsigned long oLength = *length;
  unsigned long nLength = strlen(queue->head->data);

  if (oLength < nLength) {return -2;} //buffer not large enough

  *length = oLength < nLength ? oLength : nLength;
  strncpy(buffer, queue->head->data, *length+1);
  buffer[*length] = '\0';

  free(queue->head->data);
  struct Node* temp = queue->head;
  queue->head = queue->head->next;
  free(temp);

  return 0;
}

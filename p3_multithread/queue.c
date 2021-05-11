

#include "queue.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// To create a queue
queue *queue_init(int size) {
  queue *q = (queue *)malloc(sizeof(queue));
  q->buffer_controller = malloc(size * sizeof(struct element));
  q->size = size;
  q->length = 0;
  q->head = 0;
  q->tail = 0;
  return q;
}

// To Enqueue an element, head se inserta y tail salen.
int queue_put(queue *q, struct element *x) {
  if (queue_full(q) == 0) {
    q->buffer_controller[q->head] = *x;
    q->head = (q->head + 1) % q->size;
    q->length = q->length + 1;
    return 0;
  }
  return -1;
}

// To Dequeue an element.
struct element *queue_get(queue *q) {
  struct element *elem;
  if (queue_empty(q) == 0) {
    elem = &(q->buffer_controller[q->tail]);
    q->tail = (q->tail + 1) % q->size;
    q->length = q->length - 1;
  }
  return elem;
}

// To check queue state
int queue_empty(queue *q) {
  if (q->length == 0)
    return 1;
  return 0;
}

int queue_full(queue *q) {
  if (q->length == q->size)
    return 1;
  return 0;
}

// To destroy the queue and free the resources
int queue_destroy(queue *q) {
  free(q);
  return 0;
}

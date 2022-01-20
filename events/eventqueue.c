#include "events.h"
#include "unistd.h"

void eventqueue_enqueue(EventQueue *queue, InputEvent event) {
  EventQueueItem *item = arenaalloc(queue->arena, sizeof(EventQueueItem));
  *item = (EventQueueItem){
    .event = event,
    .next = NULL,
  };

  if (queue->head == NULL) {
    // Queue empty
    queue->head = item;
    queue->tail = item;
  } else {
    // Queue not empty
    queue->tail->next = item;
    queue->tail = item;
  }
}

InputEventResult eventqueue_dequeue(EventQueue *queue) {
  if (queue->head == NULL) {
    return (InputEventResult){false};
  }

  EventQueueItem *first = queue->head;

  queue->head = first->next;
  if (queue->tail == first) {
    // Queue now empty
    queue->tail = NULL;
  }

  InputEventResult result = {
    .valid = true,
    .event = first->event,
  };
  
  // This is where you would free first

  return result;
}

bool eventqueue_isempty(EventQueue *queue) {
  return queue->head == NULL;
}

void eventqueue_clear(EventQueue *queue) {
  queue->head = NULL;
  queue->tail = NULL;
}

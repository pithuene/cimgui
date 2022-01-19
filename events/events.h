#ifndef _EVENTS_H
#define _EVENTS_H

#include <stdbool.h>
#include "../ds/mem/arenaalloc.h"

typedef struct {
  int key;
  int scancode;
  int action;
  int mods;
} KeyEvent;

typedef enum {
  InputKeyEvent,
} InputEventType;

typedef struct {
  InputEventType eventType;
  union {
    KeyEvent key;
  } ev;
} InputEvent;

// TODO: Naming?
InputEvent key_event(int key, int scancode, int action, int mods);

typedef struct EventQueueItem {
  InputEvent event;
  struct EventQueueItem *next;
} EventQueueItem;

typedef struct {
  arena_allocator_t *arena;
  EventQueueItem *head;
  EventQueueItem *tail;
} EventQueue;

/* Append an event at the end (after tail) */
void eventqueue_enqueue(EventQueue *queue, InputEvent event);

typedef struct {
  bool valid;
  InputEvent event;
} InputEventResult;

/* Remove and return the first element */
InputEventResult eventqueue_dequeue(EventQueue *queue);

#endif

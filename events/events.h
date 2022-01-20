#ifndef _EVENTS_H
#define _EVENTS_H

#include <stdbool.h>
#include "../ds/mem/arenaalloc.h"

typedef struct {} NopEvent;

typedef struct {
  int key;
  int scancode;
  int action;
  int mods;
} KeyEvent;

typedef struct {
  unsigned int codepoint;
  int mods;
} CharEvent;

typedef struct {
  int button;
  int action;
  int mods;
} MouseButtonEvent;

typedef enum {
  // Doesn't hold any information but triggers a draw since the eventqueue is not empty
  InputNopEvent,
  InputKeyEvent,
  InputCharEvent,
  InputMouseButtonEvent,
} InputEventType;

typedef struct {
  InputEventType type;
  union {
    NopEvent nop;
    KeyEvent key;
    CharEvent character;
    MouseButtonEvent mousebutton;
  } instance;
} InputEvent;

// TODO: Naming?
InputEvent nop_event(void);
InputEvent key_event(int key, int scancode, int action, int mods);
InputEvent char_event(unsigned int codepoint, int mods);
InputEvent mousebutton_event(int button, int action, int mods);

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

bool eventqueue_isempty(EventQueue *queue);

/* Remove all elements from the queue. Doesn't free the elements. */
void eventqueue_clear(EventQueue *queue);

/* Loop through all events in the queue.
 *
 * eventqueue_foreach(InputEvent event, eventqueue) {
 *   // Use event here
 * }
 */
#define eventqueue_foreach(ITEMDECL, QUEUE) \
  for (EventQueueItem *__item = QUEUE->head; __item != NULL; __item = __item->next) \
  for (ITEMDECL = __item->event, *_=(void*)1; _; _=(void*)0)

#endif

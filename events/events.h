#ifndef _EVENTS_H
#define _EVENTS_H

#include <stdbool.h>
#include "../utils/utils.h"
#include "../ds/mem/arenaalloc.h"

typedef enum {
  MouseButtonLeft   = 0,
  MouseButtonRight  = 1,
  MouseButtonMiddle = 2,
} MouseButton;

typedef enum {
  ButtonActionRelease  = 0,
  ButtonActionPress    = 1,
  ButtonActionRepeat   = 2,
  ButtonActionHeldDown = 3,
} ButtonAction;

// Doesn't hold any information but triggers a draw since the eventqueue is not empty
typedef struct {} NopEvent;

typedef struct {
  int          key;
  int          scancode;
  ButtonAction action;
  int          mods;
} KeyEvent;

typedef struct {
  unsigned int codepoint;
  int          mods;
} CharEvent;

typedef struct {
  MouseButton  button;
  DPoint       cursor;
  int          mods;
} MouseButtonPressEvent;

typedef struct {
  // The initial press event since which the button has been held down
  MouseButtonPressEvent initialPress;
  MouseButton           button;
  // Where the cursor was this frame
  DPoint                cursor;
  int                   mods;
} MouseButtonHeldDownEvent;

typedef struct {
  // The initial press event which was released
  MouseButtonPressEvent initialPress;
  MouseButton           button;
  // Where the cursor was released
  DPoint                cursor;
  int                   mods;
} MouseButtonReleaseEvent;

typedef enum {
  InputNopEvent,
  InputKeyEvent,
  InputCharEvent,
  InputMouseButtonPressEvent,
  InputMouseButtonHeldDownEvent,
  InputMouseButtonReleaseEvent,
} InputEventType;

typedef struct {
  InputEventType type;
  union {
    NopEvent nop;
    KeyEvent key;
    CharEvent character;
    MouseButtonPressEvent mousebuttonpress;
    MouseButtonHeldDownEvent mousebuttonhelddown;
    MouseButtonReleaseEvent mousebuttonrelease;
  } instance;
} InputEvent;

// TODO: Naming?
InputEvent nop_event(void);
InputEvent key_event(int key, int scancode, ButtonAction action, int mods);
InputEvent char_event(unsigned int codepoint, int mods);
InputEvent mousebuttonpress_event(MouseButton button, DPoint cursor, int mods);
InputEvent mousebuttonhelddown_event(MouseButtonPressEvent press, MouseButton button, DPoint cursor, int mods);
InputEvent mousebuttonrelease_event(MouseButtonPressEvent press, MouseButton button, DPoint cursor, int mods);

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

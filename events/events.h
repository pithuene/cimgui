#ifndef _EVENTS_H
#define _EVENTS_H

#include <stdbool.h>
#include "../utils/utils.h"
#include "../ds/mem/arenaalloc.h"
#include "../editor_core/editor_core.h"

// TODO: Rename InputEvent to just Event as this system is now used for non-input events
// TODO: Change MyType naming to my_type_t

// Mainly here to discourage using the input area outside of eventhandling,
// as the fact that it is sometimes empty and always a frame behind could
// lead to some very confusing behaviour.
typedef bbox_t input_area_t;

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
typedef char NopEvent;

typedef struct {
  int          key;
  int          scancode;
  ButtonAction action;
  int          mods;
} KeyEvent;

typedef struct {
  rune_t rune;
} CharEvent;

typedef struct {
  rune_t rune;
  int    mods;
} CharModsEvent;

typedef struct {
  MouseButton  button;
  point_t       cursor;
  int          mods;
} MouseButtonPressEvent;

typedef struct {
  // The initial press event since which the button has been held down
  MouseButtonPressEvent initialPress;
  MouseButton           button;
  // Where the cursor was this frame
  point_t                cursor;
  int                   mods;
} MouseButtonHeldDownEvent;

typedef struct {
  // The initial press event which was released
  MouseButtonPressEvent initialPress;
  MouseButton           button;
  // Where the cursor was released
  point_t                cursor;
  int                   mods;
} MouseButtonReleaseEvent;

// Used to pass information about where an area was drawn
// to the screen to the next frame.
typedef struct {
  uint64_t area_id;
  input_area_t mapped_area;
} InputAreaMappingEvent;

typedef enum {
  eventtype_nop,
  eventtype_key,
  eventtype_char,
  eventtype_charmods,
  eventtype_mousebuttonpress,
  eventtype_mousebuttonhelddown,
  eventtype_mousebuttonrelease,
  eventtype_inputareamapping,
} eventtype_t;

typedef struct {
  eventtype_t type;
  union {
    NopEvent nop;
    KeyEvent key;
    CharEvent character;
    CharModsEvent charmods;
    MouseButtonPressEvent mousebuttonpress;
    MouseButtonHeldDownEvent mousebuttonhelddown;
    MouseButtonReleaseEvent mousebuttonrelease;
    InputAreaMappingEvent inputareamapping;
  } instance;
} InputEvent;

// TODO: Naming?
InputEvent nop_event(void);
InputEvent key_event(int key, int scancode, ButtonAction action, int mods);
InputEvent char_event(rune_t codepoint);
InputEvent charmods_event(unsigned int codepoint, int mods);
InputEvent mousebuttonpress_event(MouseButton button, point_t cursor, int mods);
InputEvent mousebuttonhelddown_event(MouseButtonPressEvent press, MouseButton button, point_t cursor, int mods);
InputEvent mousebuttonrelease_event(MouseButtonPressEvent press, MouseButton button, point_t cursor, int mods);

// TODO: I think it would theoretically be a good idea to separate
// area mapping events from the event queue. If there are many clickable
// areas, there will be equally many events which every eventqueue_foreach
// will needlessly loop through.
InputEvent inputareamapping_event(uint64_t area_id, input_area_t area);

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

// Whether the eventqueue contains an input event.
// Area mapping events should not trigger a redraw.
bool eventqueue_noinput(EventQueue *queue);

/* Remove all elements from the queue. Doesn't free the elements. */
void eventqueue_clear(EventQueue *queue);

/* Loop through all events in the queue.
 *
 * eventqueue_foreach(InputEvent event, eventqueue) {
 *   // Use event here
 * }
 */
#define eventqueue_foreach(ITEMDECL, QUEUE) \
  for (EventQueueItem *__item = (QUEUE).head; __item != NULL; __item = __item->next) \
  for (ITEMDECL = __item->event, *_=(void*)1; _; _=(void*)0)

#endif

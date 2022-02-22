#ifndef _H_APPLICATION
#define _H_APPLICATION

#include <GLFW/glfw3.h>
#include "nanovg/src/nanovg.h"
#include "utils/utils.h"
#include "events/events.h"
#include "ops/ops.h"
#include "ds/mem/arenaalloc.h"

// Stores whether a mousebutton is currently pressed down and in which
// PressEvent it was initially pressed.
// Used internally to generate the InputMouseButtonHeldDownEvents
typedef struct {
  bool isDown;
  MouseButtonPressEvent press;
} _MouseButtonHeldDownState;

typedef struct {
  // Width and height of the window
	int width, height;
  // Width and height of the framebuffer
	int fbWidth, fbHeight;
  // The ratio of framebuffer width / window width for hi-dpi devices.
	float pxRatio;
} WindowInfo;

typedef struct AppContext {
	GLFWwindow *glWindow;
	NVGcontext *vg;

  // These two allocators hold per frame data and are freed after every frame.
  // They are seperated so that the events can be cleared before the operations
  // are executed, this way, events for the next frame can be created during
  // operation execution.
  arena_allocator_t event_arena;
  arena_allocator_t ops_arena;

  EventQueue eventqueue;
  oplist_t oplist;
  WindowInfo window;
  point_t cursor;
  // Time since last draw. Useful for framerate independent speed.
  double deltatime;
  _MouseButtonHeldDownState _lastMouseButtonPresses[3];
  Font font_fallback;
} AppContext;

typedef void(*AppLoopFunction)(AppContext *, void *);

typedef struct {
  AppLoopFunction draw;
} hr_guest_t;

// Execute all operations in the application oplist
void application_oplist_execute(AppContext *app);

AppContext *application_create(void);
void application_loop(
  AppContext *state,
  AppLoopFunction draw,
  void * data
);
void application_free(AppContext *state);

// Wrapper which both registers an input area for the next frame
// and retreives the mapping from the event emitted last frame.
input_area_t register_input_area(AppContext *app, point_t dimensions, uint64_t area_id);

// These helpers exist to discourage use area mappings for things despite point intersection.

bool is_cursor_in_input_area(AppContext *app, input_area_t area);
bool is_point_in_input_area(point_t point, input_area_t area);

// Calculate the position of a point relative to the topleft of an input area
point_t position_relative_to_input_area(point_t point, input_area_t area);

#endif

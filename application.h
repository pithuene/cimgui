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
} AppContext;

typedef void(*AppLoopFunction)(AppContext *, void *);

// Execute all operations in the application oplist
void application_oplist_execute(AppContext *app);

AppContext *application_create(void);
void application_loop(
  AppContext *state,
  AppLoopFunction draw,
  void * data
);
void application_free(AppContext *state);

Font application_register_font(AppContext *context, const char *name, const char *filename, float heightFactor, float heightOffset);

#endif

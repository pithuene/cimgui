#ifndef _H_APPLICATION
#define _H_APPLICATION

#include <GLFW/glfw3.h>
#include "nanovg/src/nanovg.h"
#include "events/events.h"
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
  // Holds per frame data like events. Freed after every frame.
  arena_allocator_t frameArena;
  EventQueue eventqueue;
  WindowInfo window;
  DPoint cursor;
  // Time since last draw. Useful for framerate independent speed.
  double deltatime;
  _MouseButtonHeldDownState _lastMouseButtonPresses[3];
} AppContext;

typedef void(*AppLoopFunction)(AppContext *, void *);

AppContext *application_create(void);
void application_loop(
  AppContext *state,
  AppLoopFunction draw,
  void * data
);
void application_free(AppContext *state);

#endif

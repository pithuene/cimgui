#ifndef _H_APPLICATION
#define _H_APPLICATION

#include <GLFW/glfw3.h>
#include "nanovg/src/nanovg.h"
#include "events/events.h"

typedef struct {
  // Width and height of the window
	int width, height;
  // Width and height of the framebuffer
	int fbWidth, fbHeight;
  // The ratio of framebuffer width / window width for hi-dpi devices.
	float pxRatio;
} WindowInfo;

typedef struct {
  double x;
  double y;
} DPoint;

typedef struct AppContext {
	GLFWwindow *glWindow;
	NVGcontext *vg;
  EventQueue *eventqueue;
  WindowInfo window;
  DPoint cursor;
} AppContext;

AppContext *application_create(void);
void application_loop(
  AppContext *state,
  void(*draw)(AppContext*, void *),
  void * data
);
void application_free(AppContext *state);

#endif

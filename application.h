#ifndef _H_APPLICATION
#define _H_APPLICATION

#include "nanovg/src/nanovg.h"

typedef struct {
	int winWidth, winHeight;
	int fbWidth, fbHeight;
	float pxRatio;
} WindowInfo;

typedef struct ApplicationState ApplicationState;

ApplicationState *application_create(void);
void application_loop(
  ApplicationState *state,
  void(*draw)(ApplicationState*, void *),
  void * data
);
void application_free(ApplicationState *state);

NVGcontext *application_vg(ApplicationState *state);
WindowInfo application_wininfo(struct ApplicationState *state);

#endif

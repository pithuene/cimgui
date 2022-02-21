#define _DEFAULT_SOURCE
#include <stdio.h>
#ifdef __APPLE__
#	define GLFW_INCLUDE_GLCOREARB
#endif
#include <GL/glew.h>
#define IMPLEMENTATION_UTILS_H
#include "utils/utils.h"
#include "application.h"
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>
#include "nanovg/src/nanovg.h"
#include <unistd.h>
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg/src/nanovg_gl.h"

void errorcb(int error, const char* desc) {
	printf("GLFW error %d: %s\n", error, desc);
}

static void mousebutton_callback(GLFWwindow* glWindow, int button, int action, int mods) {
  AppContext * context = glfwGetWindowUserPointer(glWindow);
  if (button >= 0 && button < 3) {
    if (action == GLFW_PRESS) {
      InputEvent pressEvent = mousebuttonpress_event(button, context->cursor, mods);
      eventqueue_enqueue(&context->eventqueue, pressEvent);
      context->_lastMouseButtonPresses[button] = (_MouseButtonHeldDownState){true, pressEvent.instance.mousebuttonpress};
    } else if (action == GLFW_RELEASE) {
      eventqueue_enqueue(&context->eventqueue, mousebuttonrelease_event(context->_lastMouseButtonPresses[button].press, button, context->cursor, mods));
      context->_lastMouseButtonPresses[button] = (_MouseButtonHeldDownState){false};
    }
  }
}

static void key_callback(GLFWwindow* glWindow, int key, int scancode, int action, int mods) {
  AppContext * context = glfwGetWindowUserPointer(glWindow);
  eventqueue_enqueue(&context->eventqueue, key_event(key, scancode, action, mods));

  // TODO: Add window close operation
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(glWindow, GL_TRUE);
}

void cursorpos_callback(GLFWwindow* window, double x, double y) {
  AppContext * context = glfwGetWindowUserPointer(window);
  eventqueue_enqueue(&context->eventqueue, nop_event());
}

void scroll_callback(GLFWwindow* window, double xoff, double yoff) {
  AppContext * context = glfwGetWindowUserPointer(window);
  eventqueue_enqueue(&context->eventqueue, nop_event());
}

void windowsize_callback(GLFWwindow* window, int width, int height) {
  AppContext * context = glfwGetWindowUserPointer(window);
  eventqueue_enqueue(&context->eventqueue, nop_event());
}

void charmods_callback(GLFWwindow* window, unsigned int codepoint, int mods) {
  AppContext * context = glfwGetWindowUserPointer(window);
  eventqueue_enqueue(&context->eventqueue, char_event(codepoint, mods));
}

struct AppContext *application_create(void) {
  struct AppContext *state = malloc(sizeof(struct AppContext));
  *state = (struct AppContext){
	  .glWindow = NULL,
	  .vg = NULL,
    ._lastMouseButtonPresses= {
      {false},
      {false},
      {false},
    },
  };

  // TODO: It should be easier to use a sensible default like pagesize
  state->event_arena = new_arena_allocator(getpagesize() - sizeof(size_t));
  state->ops_arena = new_arena_allocator(getpagesize() - sizeof(size_t));

  state->eventqueue = (EventQueue){
    .arena = &state->event_arena,
    .head = NULL,
    .tail = NULL,
  };

  state->oplist = (oplist_t){
    .arena = &state->ops_arena,
    .head = NULL,
    .tail = NULL,
  };

	if (!glfwInit()) {
		printf("Failed to init GLFW.");
    exit(1);
	}


	glfwSetErrorCallback(errorcb);
#ifndef _WIN32 // don't require this on win32, and works with more cards
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

#ifdef DEMO_MSAA
	glfwWindowHint(GLFW_SAMPLES, 4);
#endif
	state->glWindow = glfwCreateWindow(1000, 600, "NanoVG", NULL, NULL);
//	state->glWindow = glfwCreateWindow(1000, 600, "NanoVG", glfwGetPrimaryMonitor(), NULL);

  // Make the AppContext available inside the glfw callbacks
  glfwSetWindowUserPointer(state->glWindow, state);

	if (!state->glWindow) {
		glfwTerminate();
		printf("Failed to create window.");
    exit(1);
	}

  /* Set callbacks */
	glfwSetKeyCallback(state->glWindow, key_callback);
  glfwSetCharModsCallback(state->glWindow, charmods_callback);
  glfwSetMouseButtonCallback(state->glWindow, mousebutton_callback);
  glfwSetCursorPosCallback(state->glWindow, cursorpos_callback);
  glfwSetScrollCallback(state->glWindow, scroll_callback);
  glfwSetWindowSizeCallback(state->glWindow, windowsize_callback);

	glfwMakeContextCurrent(state->glWindow);
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		exit(1);
	}
	// GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();

#ifdef DEMO_MSAA
	state->vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
#else
	state->vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#endif
	if (state->vg == NULL) {
		printf("Could not init nanovg.\n");
    exit(1);
	}
	return state;
}


void application_loop(struct AppContext *context, void(*draw)(struct AppContext*, void *), void * data) {
  double t = glfwGetTime();
	double prevt = 0;
	glfwSwapInterval(0);

	glfwSetTime(0);
	prevt = glfwGetTime();

  // Forces the next frame even if there are no events
  // Set to true to trigger the initial draw.
  bool forceNextFrameDraw = true;

	while (!glfwWindowShouldClose(context->glWindow)) {
		double mx, my, dt;
		int winWidth, winHeight;
		int fbWidth, fbHeight;
		float pxRatio;
    point_t cursor;

		t = glfwGetTime();

    // Draw only if there are events to be handled or if the next frame is forced.
    if (forceNextFrameDraw || !eventqueue_isempty(&context->eventqueue)) {
      glfwGetWindowSize(context->glWindow, &winWidth, &winHeight);
      glfwGetFramebufferSize(context->glWindow, &fbWidth, &fbHeight);

      // Update and render
      glViewport(0, 0, fbWidth, fbHeight);
      // Background
      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);


      // Calculate pixel ration for hi-dpi devices.
      pxRatio = (float)fbWidth / (float)winWidth;

      nvgBeginFrame(context->vg, winWidth, winHeight, pxRatio);

      // Set cursor position
      glfwGetCursorPos(context->glWindow, &mx, &my);
      cursor = (point_t){mx, my};
      context->cursor = cursor;

      // Calculate deltatime
      dt = t - prevt;
      prevt = t;
      context->deltatime = dt,

      context->window = (WindowInfo){
        .height = winHeight,
        .width = winWidth,
        .fbHeight = fbHeight,
        .fbWidth = fbWidth,
        .pxRatio = pxRatio,
      };

      // If a frame had events to handle, force another draw on the next frame.
      // This fixes the issue of displaying data before changing it.
      //
      // Say the click of a button increments a counter, if the button is drawn
      // and therefore its events are handled *after* the counter is displayed,
      // and you only draw when there are events, the counter won't change until
      // something else emits another event.
      if (!eventqueue_isempty(&context->eventqueue)) {
        // This frame was not forced. There were actual events to handle.
        // Force a redraw next frame.
        forceNextFrameDraw = true;
      } else {
        // This frame was forced. Don't force another.
        forceNextFrameDraw = false;
      }

      // Call the draw function.
      // Handles all events, executes the layout logic and populates the oplist
      (*draw)(context, data);

      // Clear events from last frame.
      // Occurs before the oplist execution so events for the next frame can
      // be created during execution.
      eventqueue_clear(&context->eventqueue);
      arena_allocator_reset(&context->event_arena);

      //oplist_print(&context->oplist);
      application_oplist_execute(context);

      nvgEndFrame(context->vg);
      glfwSwapBuffers(context->glWindow);

      oplist_clear(&context->oplist);
      arena_allocator_reset(&context->ops_arena);
    }

    /* Limit FPS */
    double endTime = glfwGetTime();
    double targetFrameTime = 1000000/60;
    double frameTime = (endTime - t) * 1000000;
    double sleepTime = targetFrameTime - frameTime;

    // Drawing takes so long, that FPS is below target anyways.
    // No sleep required.
    if (sleepTime > 0) {
      usleep(sleepTime);
    }

    _MouseButtonHeldDownState IsMouseButtonHeldDownBefore[3] = {
      context->_lastMouseButtonPresses[0],
      context->_lastMouseButtonPresses[1],
      context->_lastMouseButtonPresses[2],
    };

		glfwPollEvents();

    // Generate MouseButtonHeldDown events
    for (int8_t i = 0; i < 3; i++) {
      if (IsMouseButtonHeldDownBefore[i].isDown && context->_lastMouseButtonPresses[i].isDown) {
        // TODO: Fix mods
        eventqueue_enqueue(&context->eventqueue, mousebuttonhelddown_event(context->_lastMouseButtonPresses[i].press, i, cursor, 0));
      }
    }
	}
}

void application_free(struct AppContext *state) {
	nvgDeleteGL3(state->vg);
	glfwTerminate();
  free(state);
}

// Execute all operations in an oplist
void application_oplist_execute(AppContext *app) {
  op_execution_state_t exec_state = {
    .vg = app->vg,
    .offset = {0,0},
  };
  for (oplist_item_t *item = app->oplist.head; item != NULL; item = item->next) {
    op_execute(&exec_state, item->op);
  }
}

Font application_register_font(AppContext *context, const char *name, const char *filename, float heightFactor, float heightOffset) {
  Font font = {
    .name = name,
    .heightFactor = heightFactor,
    .heightOffset = heightOffset,
  };
	font.handle = nvgCreateFont(context->vg, name, filename);
  return font;
}

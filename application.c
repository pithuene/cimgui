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
#include "checktag/checktag.h"
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "widgets/widgets.h"

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

  // TODO: Fix scaling factors
  state->font_fallback = load_font(state->vg, "sans", 0.685, 0.115);

	return state;
}

typedef struct {
  void *curr_handle;
  AppLoopFunction draw;
  time_t loaded_last_modified;
} hot_reload_state_t;

void empty_app_loop_function(AppContext * app, void * data) {
  bbox_t window_bounds = bbox_from_dims((point_t){0,0}, app->window.width, app->window.height);
  rect(app, window_bounds, &(rect_t){.color = {255,83,83,255}});
  op_offset(&app->oplist, (point_t){50,50});
  text(app, window_bounds, &(text_t){
    .color = (color_t){0,0,0,255},
    .content = "No draw function loaded currently.",
    .font = &app->font_fallback,
    .size = 20,
  });
  op_offset(&app->oplist, (point_t){0,30});
  text(app, window_bounds, &(text_t){
    .color = (color_t){0,0,0,255},
    .content = "Either waiting for hot reloading or the hot reloading module was not found.",
    .font = &app->font_fallback,
    .size = 20,
  });
}

// Returns whether a new version was loaded
static bool hot_code_load(hot_reload_state_t *hrs) {
  const char *so_path = "./render.so";

  // Get file stats of shared object
  struct stat so_stats;
  if (stat(so_path, &so_stats) == -1) {
    perror(so_path);
    exit(1);
  }

  if (so_stats.st_size <= 0) {
    // During recompilation the file size is zero before the write is done.
    return false;
  }

  time_t last_modified = so_stats.st_mtime;
  if (!(last_modified > hrs->loaded_last_modified)) {
    // Current version already loaded
    return false;
  }

  // Unload current version
  hrs->draw = empty_app_loop_function;
  if (hrs->curr_handle) {
    dlclose(hrs->curr_handle);
    hrs->curr_handle = NULL;
  }

  void *new_handle = dlopen(so_path, RTLD_NOW);
  if (new_handle) {
    hr_guest_t *hr_guest = (hr_guest_t *) dlsym(new_handle, "hr_guest_draw");
    AppLoopFunction new_draw = hr_guest->draw;
    if (new_draw) {
      hrs->draw = new_draw;
      hrs->curr_handle = new_handle;
      hrs->loaded_last_modified = last_modified;
    } else {
      printf("new_draw undefined\n");
    }
  } else {
    printf("new_handle undefined: %s\n", dlerror());
    printf("file size is: %lo\n", so_stats.st_size);
  }
  return true;
}


void application_loop(struct AppContext *context, AppLoopFunction initial_draw, void * data) {
  double t = glfwGetTime();
	double prevt = 0;
	glfwSwapInterval(0);

	glfwSetTime(0);
	prevt = glfwGetTime();

  hot_reload_state_t hr_state = {
    .curr_handle = NULL,
    .draw = empty_app_loop_function,
  };

  // Forces the next frame even if there are no events
  // Set to true to trigger the initial draw.
  bool forceNextFrameDraw = true;

	while (!glfwWindowShouldClose(context->glWindow)) {
    bool hot_reload_performed = hot_code_load(&hr_state);

    if (hot_reload_performed) {
      // Force a redraw if a new version was linked in
      eventqueue_enqueue(&context->eventqueue, nop_event());
    }

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

      set_draw_stack_start(&context);
      // Call the draw function.
      // Handles all events, executes the layout logic and populates the oplist
      (*hr_state.draw)(context, data);

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


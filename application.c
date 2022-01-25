#include "application.h"
#include <stdio.h>
#ifdef __APPLE__
#	define GLFW_INCLUDE_GLCOREARB
#endif
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>
#include "nanovg/src/nanovg.h"
#include <unistd.h>
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg/src/nanovg_gl.h"
#include "ds/mem/arenaalloc.h"

// Holds per frame data like events
// Freed after every frame
arena_allocator_t frameArena;

typedef struct {
  bool isDown;
  // The initial click was made
  MouseButtonPressEvent press;
} MouseButtonHeldDownState;

// TODO: Name: LastMouseButtonState ???
MouseButtonHeldDownState LastMouseButtonPresses[3] = {
  {false},
  {false},
  {false},
};

void errorcb(int error, const char* desc) {
	printf("GLFW error %d: %s\n", error, desc);
}

static void mousebutton_callback(GLFWwindow* glWindow, int button, int action, int mods) {
  AppContext * context = glfwGetWindowUserPointer(glWindow);
  if (button >= 0 && button < 3) {
    if (action == GLFW_PRESS) {
      InputEvent pressEvent = mousebuttonpress_event(button, context->cursor, mods);
      eventqueue_enqueue(&context->eventqueue, pressEvent);
      LastMouseButtonPresses[button] = (MouseButtonHeldDownState){true, pressEvent.instance.mousebuttonpress};
    } else if (action == GLFW_RELEASE) {
      eventqueue_enqueue(&context->eventqueue, mousebuttonrelease_event(LastMouseButtonPresses[button].press, button, context->cursor, mods));
      LastMouseButtonPresses[button] = (MouseButtonHeldDownState){false};
    }
  }
}

static void key_callback(GLFWwindow* glWindow, int key, int scancode, int action, int mods) {
  AppContext * context = glfwGetWindowUserPointer(glWindow);
  eventqueue_enqueue(&context->eventqueue, key_event(key, scancode, action, mods));
  
	NVG_NOTUSED(scancode);
	NVG_NOTUSED(mods);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(glWindow, GL_TRUE);
	/*if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		blowup = !blowup;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		screenshot = 1;
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		premult = !premult;*/
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
  };

  // TODO: It should be easier to use a sensible default like pagesize
  frameArena = new_arena_allocator(getpagesize() - sizeof(size_t));

  state->eventqueue = (EventQueue){
    .arena = &frameArena,
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
#ifdef NANOVG_GLEW
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		return -1;
	}
	// GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();
#endif

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


void application_loop(struct AppContext *state, void(*draw)(struct AppContext*, void *), void * data) {
	double prevt = 0;
	glfwSwapInterval(0);

	glfwSetTime(0);
	prevt = glfwGetTime();

  // Trigger initial draw
  eventqueue_enqueue(&state->eventqueue, nop_event());

	while (!glfwWindowShouldClose(state->glWindow))
	{
		double mx, my, t, dt;
		int winWidth, winHeight;
		int fbWidth, fbHeight;
		float pxRatio;
		float gpuTimes[3];
		int i, n;
    DPoint cursor;

		t = glfwGetTime();
		dt = t - prevt;
		prevt = t;

		glfwGetCursorPos(state->glWindow, &mx, &my);
		glfwGetWindowSize(state->glWindow, &winWidth, &winHeight);
		glfwGetFramebufferSize(state->glWindow, &fbWidth, &fbHeight);

    cursor = (DPoint){mx, my};

		// Calculate pixel ration for hi-dpi devices.
		pxRatio = (float)fbWidth / (float)winWidth;

    if (!eventqueue_isempty(&state->eventqueue)) {
      // Update and render
      glViewport(0, 0, fbWidth, fbHeight);
      // Background
      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

      nvgBeginFrame(state->vg, winWidth, winHeight, pxRatio);

      state->window = (WindowInfo){
        .height = winHeight,
        .width = winWidth,
        .fbHeight = fbHeight,
        .fbWidth = fbWidth,
        .pxRatio = pxRatio
      };
      state->cursor = cursor;

      // Call the draw function
      (*draw)(state, data);

      nvgEndFrame(state->vg);
      glfwSwapBuffers(state->glWindow);

      eventqueue_clear(&state->eventqueue);
      arena_allocator_reset(&frameArena);
    }

    /* Limit FPS */
    double endTime = glfwGetTime();
    double targetFrameTime = 1000000/60;
    double frameTime = (endTime - t) * 1000000;
    usleep(targetFrameTime - frameTime);

    MouseButtonHeldDownState IsMouseButtonHeldDownBefore[3] = {
      LastMouseButtonPresses[0],
      LastMouseButtonPresses[1],
      LastMouseButtonPresses[2],
    };

		glfwPollEvents();

    for (int8_t i = 0; i < 3; i++) {
      if (IsMouseButtonHeldDownBefore[i].isDown && LastMouseButtonPresses[i].isDown) {
        // TODO: Fix mods
        eventqueue_enqueue(&state->eventqueue, mousebuttonhelddown_event(LastMouseButtonPresses[i].press, i, cursor, 0));
      }
    }
	}
}

void application_free(struct AppContext *state) {
	nvgDeleteGL3(state->vg);
	glfwTerminate();
  free(state);
}

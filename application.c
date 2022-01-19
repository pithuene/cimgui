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

EventQueue eventqueue;

void errorcb(int error, const char* desc) {
	printf("GLFW error %d: %s\n", error, desc);
}

static void key(GLFWwindow* glWindow, int key, int scancode, int action, int mods) {
  eventqueue_enqueue(&eventqueue, key_event(key, scancode, action, mods));
  
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

struct AppContext *application_create(void) {
  struct AppContext *state = malloc(sizeof(struct AppContext));
  *state = (struct AppContext){
	  .glWindow = NULL,
	  .vg = NULL,
    .eventqueue = &eventqueue,
  };

  // TODO: It should be easier to use a sensible default like pagesize
  frameArena = new_arena_allocator(getpagesize() - sizeof(size_t));
  eventqueue = (EventQueue){
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
	if (!state->glWindow) {
		glfwTerminate();
		printf("Failed to create window.");
    exit(1);
	}

	glfwSetKeyCallback(state->glWindow, key);

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

	while (!glfwWindowShouldClose(state->glWindow))
	{
		double mx, my, t, dt;
		int winWidth, winHeight;
		int fbWidth, fbHeight;
		float pxRatio;
		float gpuTimes[3];
		int i, n;

		t = glfwGetTime();
		dt = t - prevt;
		prevt = t;

		glfwGetCursorPos(state->glWindow, &mx, &my);
		glfwGetWindowSize(state->glWindow, &winWidth, &winHeight);
		glfwGetFramebufferSize(state->glWindow, &fbWidth, &fbHeight);
		// Calculate pixel ration for hi-dpi devices.
		pxRatio = (float)fbWidth / (float)winWidth;

		// Update and render
		glViewport(0, 0, fbWidth, fbHeight);
		glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

		nvgBeginFrame(state->vg, winWidth, winHeight, pxRatio);

    state->window = (WindowInfo){
      .height = winHeight,
      .width = winWidth,
      .fbHeight = fbHeight,
      .fbWidth = fbWidth,
      .pxRatio = pxRatio
    };
    state->cursor = (DPoint){mx, my};

    // Call the draw function
    (*draw)(state, data);

		nvgEndFrame(state->vg);
		glfwSwapBuffers(state->glWindow);

    eventqueue_clear(&eventqueue);
    arena_allocator_reset(&frameArena);

    /* Limit FPS */
    double endTime = glfwGetTime();
    double targetFrameTime = 1000000/60;
    double frameTime = (endTime - t) * 1000000;
    usleep(targetFrameTime - frameTime);

		glfwPollEvents();
	}
}

void application_free(struct AppContext *state) {
	nvgDeleteGL3(state->vg);
	glfwTerminate();
  free(state);
}

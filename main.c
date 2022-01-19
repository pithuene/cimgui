#include "application.h"
#include "nanovg/src/nanovg.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include "events/events.h"
#include "ds/ds/vec.h"

int fontNormal = -1;

void drawEyes(NVGcontext* vg, float x, float y, float w, float h, float mx, float my, float t)
{
	NVGpaint gloss, bg;
	float ex = w *0.23f;
	float ey = h * 0.5f;
	float lx = x + ex;
	float ly = y + ey;
	float rx = x + w - ex;
	float ry = y + ey;
	float dx,dy,d;
	float br = (ex < ey ? ex : ey) * 0.5f;
	float blink = 1 - pow(sinf(t*0.5f),200)*0.8f;

	bg = nvgLinearGradient(vg, x,y+h*0.5f,x+w*0.1f,y+h, nvgRGBA(0,0,0,32), nvgRGBA(0,0,0,16));
	nvgBeginPath(vg);
	nvgEllipse(vg, lx+3.0f,ly+16.0f, ex,ey);
	nvgEllipse(vg, rx+3.0f,ry+16.0f, ex,ey);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	bg = nvgLinearGradient(vg, x,y+h*0.25f,x+w*0.1f,y+h, nvgRGBA(220,220,220,255), nvgRGBA(128,128,128,255));
	nvgBeginPath(vg);
	nvgEllipse(vg, lx,ly, ex,ey);
	nvgEllipse(vg, rx,ry, ex,ey);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	dx = (mx - rx) / (ex * 10);
	dy = (my - ry) / (ey * 10);
	d = sqrtf(dx*dx+dy*dy);
	if (d > 1.0f) {
		dx /= d; dy /= d;
	}
	dx *= ex*0.4f;
	dy *= ey*0.5f;
	nvgBeginPath(vg);
	nvgEllipse(vg, lx+dx,ly+dy+ey*0.25f*(1-blink), br,br*blink);
	nvgFillColor(vg, nvgRGBA(32,32,32,255));
	nvgFill(vg);

	dx = (mx - rx) / (ex * 10);
	dy = (my - ry) / (ey * 10);
	d = sqrtf(dx*dx+dy*dy);
	if (d > 1.0f) {
		dx /= d; dy /= d;
	}
	dx *= ex*0.4f;
	dy *= ey*0.5f;
	nvgBeginPath(vg);
	nvgEllipse(vg, rx+dx,ry+dy+ey*0.25f*(1-blink), br,br*blink);
	nvgFillColor(vg, nvgRGBA(32,32,32,255));
	nvgFill(vg);

	gloss = nvgRadialGradient(vg, lx-ex*0.25f,ly-ey*0.5f, ex*0.1f,ex*0.75f, nvgRGBA(255,255,255,128), nvgRGBA(255,255,255,0));
	nvgBeginPath(vg);
	nvgEllipse(vg, lx,ly, ex,ey);
	nvgFillPaint(vg, gloss);
	nvgFill(vg);

	gloss = nvgRadialGradient(vg, rx-ex*0.25f,ry-ey*0.5f, ex*0.1f,ex*0.75f, nvgRGBA(255,255,255,128), nvgRGBA(255,255,255,0));
	nvgBeginPath(vg);
	nvgEllipse(vg, rx,ry, ex,ey);
	nvgFillPaint(vg, gloss);
	nvgFill(vg);
}

char text[200] = "Start: ";
int textlen = 7;

void draw(AppContext *app, void * data) {
  drawEyes(app->vg, app->window.width - 250, 50, 150, 100, 0, 0, 0);

  nvgBeginPath(app->vg);
  nvgRect(app->vg, app->cursor.x, app->cursor.y, 30, 30);
  nvgFillColor(app->vg, nvgRGBA(255,192,0,255));
  nvgFill(app->vg);

  InputEventResult ev;
  while ((ev = eventqueue_dequeue(app->eventqueue)).valid) {
    if (ev.event.eventType == InputKeyEvent && ev.event.ev.key.action == GLFW_PRESS) {
      text[textlen] = ev.event.ev.key.key;
      textlen++;
    }
  }

  /* Print number of arena containers */
  char arena_containers[30];
  sprintf(arena_containers, "Arena conts: %lu", veclen(app->eventqueue->arena->containers));
	nvgFontSize(app->vg, 20);
	nvgFontFace(app->vg, "sans");
	nvgFillColor(app->vg, nvgRGBA(255,255,255,255));
	nvgTextAlign(app->vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
	nvgText(app->vg, 200,200, arena_containers, NULL);

  /* Print text */
	nvgFontSize(app->vg, 20);
	nvgFontFace(app->vg, "sans");
	nvgFillColor(app->vg, nvgRGBA(255,255,255,255));
	nvgTextAlign(app->vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
	nvgText(app->vg, 200,100, text, NULL);
}

int main(void) {
  int counter = 5;

  AppContext *app = application_create();

	fontNormal = nvgCreateFont(app->vg, "sans", "nanovg/example/Roboto-Regular.ttf");
	if (fontNormal == -1) {
		printf("Could not add font italic.\n");
		return -1;
	}

  application_loop(app, draw, &counter);
  application_free(app);
}

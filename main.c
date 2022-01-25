#include "application.h"
#include "nanovg/src/nanovg.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include "events/events.h"
#include "widgets/widgets.h"

int fontNormal = -1;

char text[200] = "Start: ";
int textlen = 7;
int btncount = 0;

void draw(AppContext *app, void * data) {
  eventqueue_foreach(InputEvent event, app->eventqueue) {
    if (event.type == InputCharEvent) {
      unsigned int codepoint = event.instance.character.codepoint;
      text[textlen] = codepoint;
      textlen++;
    }
  }

  ButtonConfig btnconf = {
    .x = 50,
    .y = 50,
    .label = "Click me!",
    .background = nvgRGBA(255,0,0,255),
    .background_hover = nvgRGBA(0,255,0,255),
    .background_down = nvgRGBA(0,0,255,255),
  };
  if (button(app, btnconf)) {
    btncount++;
  }

  /* Print btncount */
  char btn_count_str[40];
  sprintf(btn_count_str, "Button has been clicked: %d times", btncount);
	nvgFontSize(app->vg, 20);
	nvgFontFace(app->vg, "sans");
	nvgFillColor(app->vg, nvgRGBA(0,0,0,255));
	nvgTextAlign(app->vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
	nvgText(app->vg, 200,200, btn_count_str, NULL);

  /* Print text */
	nvgFontSize(app->vg, 20);
	nvgFontFace(app->vg, "sans");
	nvgFillColor(app->vg, nvgRGBA(0,0,0,255));
	nvgTextAlign(app->vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
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

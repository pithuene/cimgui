#include "application.h"
#include "nanovg/src/nanovg.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include "events/events.h"
#include "widgets/widgets.h"
#include "font/font.h"

Font fontNormal = {0};

char text[200] = "Ttart: ";
int textlen = 7;
int fontSize = 90;

void draw(AppContext *app, void * data) {
  eventqueue_foreach(InputEvent event, app->eventqueue) {
    if (event.type == InputCharEvent) {
      unsigned int codepoint = event.instance.character.codepoint;
      text[textlen] = codepoint;
      textlen++;
    }
  }

  /* Print btncount */
  char btn_count_str[40];
  sprintf(btn_count_str, "Button has been clicked: %d times", fontSize);
	nvgFontSize(app->vg, 20);
	nvgFontFace(app->vg, "sans");
	nvgFillColor(app->vg, nvgRGBA(0,0,0,255));
	nvgTextAlign(app->vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
	nvgText(app->vg, 200,200, btn_count_str, NULL);

  bool increase_clicked = false;
  bool decrease_clicked = false;
  ButtonConfig increase_btnconf = {
    .result = &increase_clicked,
    .x = 50,
    .y = 50,
    .label = "Increase",
    .background = nvgRGBA(170, 170, 170, 255),
    .background_hover = nvgRGBA(190, 190, 190, 255),
    .background_down = nvgRGBA(150, 150, 150, 255),
  };
  Widget btn_increase = button(&increase_btnconf);

  ButtonConfig decrease_btnconf = {
    .result = &decrease_clicked,
    .x = 200,
    .y = 50,
    .label = "Decrease",
    .background = nvgRGBA(170, 170, 170, 255),
    .background_hover = nvgRGBA(190, 190, 190, 255),
    .background_down = nvgRGBA(150, 150, 150, 255),
  };
  Widget btn_decrease = button(&decrease_btnconf);

  draw_widget(app, btn_increase);
  draw_widget(app, btn_decrease);
  if (increase_clicked) {
    fontSize += 5;
  }
  if (decrease_clicked) {
    fontSize -= 5;
  }

  double heightFactor = 0.71;
  double heightOffset = fontSize*0.0805;
  nvgBeginPath(app->vg);
  //nvgRect(app->vg, 200, 100+heightOffset, fontSize, fontSize*heightFactor);
  nvgRect(app->vg, 200, 100, fontSize, fontSize);
  nvgFillColor(app->vg, nvgRGBA(255, 0, 0, 255));
  nvgFill(app->vg);

  /* Print text */
	nvgFillColor(app->vg, nvgRGBA(0,0,0,255));
	nvgTextAlign(app->vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
  draw_text(app, &fontNormal, fontSize, (DPoint){200, 100}, text);
}

int main(void) {
  int counter = 5;

  AppContext *app = application_create();

  fontNormal = register_font(app, "sans", "/home/pit/code/nanovg-test/nanovg/example/Roboto-Regular.ttf", 0.71, 0.11);
	/*fontNormal = nvgCreateFont(app->vg, "sans", "/home/pit/code/nanovg-test/nanovg/example/Roboto-Regular.ttf");
	if (fontNormal == -1) {
		printf("Could not add font italic.\n");
		return -1;
	}*/

  application_loop(app, draw, &counter);
  application_free(app);
}

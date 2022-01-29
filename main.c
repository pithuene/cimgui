#include "application.h"
#include "nanovg/src/nanovg.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include "events/events.h"
#include "widgets/widgets.h"
#include "font/font.h"

Font fontNormal = {0};

char text[200] = "Start: ";
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
  TextConfig label = {
    .font = &fontNormal,
    .size = 20,
    .position = {
      .x = 200,
      .y = 200,
    },
    .content = btn_count_str,
    .color = nvgRGB(0, 0, 0),
  };

  DPoint label_size = widg_text.size(app, &label);
  RectConfig labelBackground = {
    .color = nvgRGB(255,0,0),
    .x = 200,
    .y = 200,
    .w = label_size.x,
    .h = label_size.y,
  };
  widg_rect.draw(app, &labelBackground);

  widg_text.draw(app, &label);

  bool increase_clicked = false;
  bool decrease_clicked = false;

  ButtonConfig increase_btnconf = {
    .result           = &increase_clicked,
    .x                = 50,
    .y                = 50,
    .label            = "Increase",
    .label_font       = &fontNormal,
    .label_font_size  = 14,
    .label_color      = nvgRGBA(0, 0, 0, 255),
    .background       = nvgRGBA(170, 170, 170, 255),
    .background_hover = nvgRGBA(190, 190, 190, 255),
    .background_down  = nvgRGBA(150, 150, 150, 255),
  };

  ButtonConfig decrease_btnconf = {
    .result           = &decrease_clicked,
    .x                = 200,
    .y                = 50,
    .label            = "Decrease",
    .label_font       = &fontNormal,
    .label_font_size  = 14,
    .label_color      = nvgRGBA(0, 0, 0, 255),
    .background       = nvgRGBA(170, 170, 170, 255),
    .background_hover = nvgRGBA(190, 190, 190, 255),
    .background_down  = nvgRGBA(150, 150, 150, 255),
  };

  widg_button.draw(app, &increase_btnconf);
  widg_button.draw(app, &decrease_btnconf);
  if (increase_clicked) {
    fontSize += 5;
  }
  if (decrease_clicked) {
    fontSize -= 5;
  }

  double heightFactor = 0.71;
  double heightOffset = fontSize*0.0805;
  nvgBeginPath(app->vg);
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

  application_loop(app, draw, &counter);
  application_free(app);
}

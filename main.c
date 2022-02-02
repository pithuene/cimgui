#include "application.h"
#include "nanovg/src/nanovg.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include "events/events.h"
#include "widgets/widgets.h"
#include "font/font.h"

typedef struct {
  Font fontNormal;
  char text[200];
  int textlen;
  int fontSize;
} State;

void draw(AppContext *app, State *state) {
  eventqueue_foreach(InputEvent event, app->eventqueue) {
    if (event.type == InputCharEvent) {
      unsigned int codepoint = event.instance.character.codepoint;
      state->text[state->textlen] = codepoint;
      state->textlen++;
    }
  }

  /* Print btncount */
  char btn_count_str[40];
  sprintf(btn_count_str, "Button has been clicked: %d times", state->fontSize);
  widget_t *label = text(
    .font = &state->fontNormal,
    .size = 10,
    .position = {
      .x = 200,
      .y = 200,
    },
    .content = btn_count_str,
    .color = nvgRGB(0, 0, 0),
  );

  DPoint label_size = widget_getsize(app, label);

  widget_draw(app, rect(
    .color = nvgRGB(255,255,255),
    .x = 200,
    .y = 200,
    .w = label_size.x,
    .h = label_size.y,
  ));

  widget_draw(app, label);

  bool increase_clicked = false;
  widget_draw(app, button(
    .result           = &increase_clicked,
    .x                = 20,
    .y                = 50,
    .label            = "Increase",
    .label_font       = &state->fontNormal,
    .label_font_size  = 14,
    .label_color      = nvgRGBA(0, 0, 0, 255),
    .background       = nvgRGBA(170, 170, 170, 255),
    .background_hover = nvgRGBA(190, 190, 190, 255),
    .background_down  = nvgRGBA(150, 150, 150, 255),
  ));
  if (increase_clicked) {
    state->fontSize += 5;
  }

  bool decrease_clicked = false;
  widget_draw(app, button(
    .result           = &decrease_clicked,
    .x                = 200,
    .y                = 50,
    .label            = "Decrease",
    .label_font       = &state->fontNormal,
    .label_font_size  = 14,
    .label_color      = nvgRGBA(0, 0, 0, 255),
    .background       = nvgRGBA(170, 170, 170, 255),
    .background_hover = nvgRGBA(190, 190, 190, 255),
    .background_down  = nvgRGBA(150, 150, 150, 255),
  ));
  if (decrease_clicked) {
    state->fontSize -= 5;
  }

  nvgBeginPath(app->vg);
  nvgRect(app->vg, 200, 100, state->fontSize, state->fontSize);
  nvgFillColor(app->vg, nvgRGBA(255, 0, 0, 255));
  nvgFill(app->vg);

  /* Print text */
	nvgFillColor(app->vg, nvgRGBA(0,0,0,255));
	nvgTextAlign(app->vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
  draw_text(app, &state->fontNormal, state->fontSize, (DPoint){200, 100}, state->text);
}

int main(void) {
  AppContext *app = application_create();

  State state =  {
    .text = "Start: ",
    .textlen = 7,
    .fontSize = 90,
  };

  state.fontNormal = register_font(app, "sans", "/home/pit/code/nanovg-test/nanovg/example/Roboto-Regular.ttf", 0.71, 0.11);

  application_loop(app, (AppLoopFunction) draw, &state);
  application_free(app);
}

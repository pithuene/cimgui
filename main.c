#include "application.h"
#include "nanovg/src/nanovg.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <fontconfig/fontconfig.h>
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

  widget_draw(
    app,
    (bbox_t){
      .min = {0,0},
      .max = {200,200},
    },
    rect(.color = nvgRGB(255,255,255))
  );

  /* Print btncount */
  char btn_count_str[40];
  sprintf(btn_count_str, "Button has been clicked: %d times", state->fontSize);
  widget_t *label = text(
    .font = &state->fontNormal,
    .size = 10,
    .content = btn_count_str,
  );

  point_t label_size = widget_getsize(app, CONSTRAINT_NONE, label);

  bbox_t label_bounds = bbox_from_dims((point_t){200, 200}, label_size.x, label_size.y);
  widget_draw(
    app,
    label_bounds,
    rect(.color = nvgRGB(255,255,255))
  );

  widget_draw(app, label_bounds, label);

  bool increase_clicked = false;
  
  widget_draw(
    app,
    (bbox_t){
      .min = {50, 50},
      .max = {50, 50},
    },
    button(
      .result           = &increase_clicked,
      .label            = "Increase",
      .label_font       = &state->fontNormal,
      .label_font_size  = 14,
      .label_color      = nvgRGBA(0, 0, 0, 255),
      .background       = nvgRGBA(170, 170, 170, 255),
      .background_hover = nvgRGBA(190, 190, 190, 255),
      .background_down  = nvgRGBA(150, 150, 150, 255),
    )
  );
  if (increase_clicked) {
    state->fontSize += 5;
  }

  bool decrease_clicked = false;
  widget_draw(
    app,
    (bbox_t){
      .min = {200, 50},
      .max = {200, 50},
    },
    button(
      .result           = &decrease_clicked,
      .label            = "Decrease",
      .label_font       = &state->fontNormal,
      .label_font_size  = 14,
      .label_color      = nvgRGBA(0, 0, 0, 255),
      .background       = nvgRGBA(170, 170, 170, 255),
      .background_hover = nvgRGBA(190, 190, 190, 255),
      .background_down  = nvgRGBA(150, 150, 150, 255),
    )
  );
  if (decrease_clicked) {
    state->fontSize -= 5;
  }

  static int slider_value = 50;

  widget_draw(
    app,
    (bbox_t){
      .min = {300, 270},
      .max = {300, 270},
    },
    slider(
      .font = &state->fontNormal,
      .min = 0,
      .max = 100,
      .step = 1,
      .value = &slider_value,
    )
  );

  char minlabel[10];
  char maxlabel[10];
  sprintf(minlabel, "%d", 5);
  sprintf(maxlabel, "%d", 100);

  widget_draw(
    app,
    (bbox_t){
      .min = {300, 300},
      .max = {600, 400},
    },
    row(
      .item_count = 3,
      .items = (widget_t*[]){
        text(
          .font = &state->fontNormal,
          .size = 10,
          .content = minlabel,
        ),
        slider(
          .font = &state->fontNormal,
          .min = 5,
          .max = 100,
          .step = 1,
          .value = &state->fontSize,
        ),
        text(
          .font = &state->fontNormal,
          .size = 10,
          .content = maxlabel,
        ),
      }
    )
  );

  widget_t *myrow = row(
    .item_count = 3,
    .items = (widget_t*[]){
      rect(
        .color = nvgRGB(0,255,255),
      ),
      circle(
        .color = nvgRGB(255,255,0),
      ),
      rect(
        .color = nvgRGB(255,0,255),
      ),
    },
    .spacing = slider_value,
  );

  widget_t *mystack = stack(
    .item_count = 2,
    .items = (widget_t*[]){
      rect(
        .color = nvgRGB(0,255,255),
      ),
      center(
        .child = circle(
          .color = nvgRGB(255,255,0),
        ),
      ),
    },
    .spacing = slider_value,
  );

  /* Print slider_value */
  sprintf(btn_count_str, "Slider value: %d", slider_value);
  widget_draw(
    app,
    (bbox_t){
      .min = {300, 250},
      .max = {300, 250},
    },
    text(
      .font = &state->fontNormal,
      .size = 10,
      .content = btn_count_str,
    )
  );

  widget_draw(
    app,
    (bbox_t){
      .min = {500, 500},
      .max = {900, 600},
    },
    myrow
  );

  widget_draw(
    app,
    (bbox_t){
      .min = {800, 800},
      .max = {1200, 900},
    },
    mystack
  );

  nvgBeginPath(app->vg);
  nvgRect(app->vg, 200, 100, state->fontSize, state->fontSize);
  nvgFillColor(app->vg, nvgRGBA(255, 0, 0, 255));
  nvgFill(app->vg);

  /* Print text */
	nvgFillColor(app->vg, nvgRGBA(0,0,0,255));
	nvgTextAlign(app->vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
  draw_text(app, &state->fontNormal, state->fontSize, (point_t){200, 100}, state->text);
}

// Given a pattern, returns the path to a font file.
// The path is heap allocated and must be freed.
Font load_font(AppContext *app, char * input_pattern, float heightFactor, float heightOffset) {
  FcInit();
  FcConfig *config = FcInitLoadConfigAndFonts();
  FcPattern *pattern = FcNameParse((const FcChar8*)input_pattern);
  FcConfigSubstitute(config, pattern, FcMatchPattern);
  FcDefaultSubstitute(pattern);

  char *fontFile = NULL;
  Font font = {0};
  FcResult result;
  FcPattern *font_pattern = FcFontMatch(config, pattern, &result);
  if (font_pattern) {
    FcChar8 *file = NULL;
    if (FcPatternGetString(font_pattern, FC_FILE, 0, &file) == FcResultMatch) {
      fontFile = (char*) file;
      font = register_font(app, input_pattern, fontFile, heightFactor, heightOffset);
    } else {
      printf("No font matches!\n");
      exit(1);
    }
  } else {
    printf("Font pattern not found!\n");
    exit(1);
  }

  FcPatternDestroy(font_pattern);
  FcPatternDestroy(pattern);
  FcConfigDestroy(config);
  FcFini();
  return font;
}

int main(void) {
  AppContext *app = application_create();

  State state =  {
    .text = "HItart: ",
    .textlen = 7,
    .fontSize = 90,
  };

  state.fontNormal = load_font(app, "monospace", 0.685, 0.115);
  //state.fontNormal = register_font(app, "sans", "/home/pit/code/nanovg-test/nanovg/example/Roboto-Regular.ttf", 0.71, 0.11);

  application_loop(app, (AppLoopFunction) draw, &state);
  application_free(app);
}

#include "application.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <fontconfig/fontconfig.h>
#include "events/events.h"
#include "widgets/widgets.h"
#include "element/element.h"
#include "font/font.h"
#include "checktag/checktag.h"

typedef struct {
  Font fontNormal;
  char text[200];
  int textlen;
  int fontSize;
  button_t btns[3];
} State;


void draw(AppContext *app, State *state) {
  check_tag(&state->fontSize);

  bbox_t window_bounds = bbox_from_dims((point_t){0,0}, app->window.width, app->window.height);
  deferred_draw_t d = widget_draw_deferred(
    app,
    window_bounds,
    &(widget_t){
      .draw = (widget_draw_t) text,
      .data = &(text_t){
        .color = (color_t){0, 0, 0, 255},
        .content = "Test",
        .font = &state->fontNormal,
        .size = 30,
      }
    }
  );
  rect(app, (bbox_t){{0}, d.dimensions}, &(rect_t){.color = {200,200,200,255}});
  deferred_draw_execute(app, d);
  op_offset(&app->oplist, (point_t){100, 100});

  button_t button_template = {
    .background = {200,200,200,255},
    .background_down = {180,180,180,255},
    .background_hover = {210, 210, 210, 255},
    .label_color = {0,0,0,255},
    .label_font = &state->fontNormal,
    .label_font_size = 10,
  };

  bool res1 = false;
  state->btns[0] = button_template;
  state->btns[0].label = "Button 1";
  state->btns[0].result = &res1;
  button(app, window_bounds, &state->btns[0]);

  op_offset(&app->oplist, (point_t){100, 0});

  bool res2 = false;
  state->btns[1] = button_template;
  state->btns[1].label = "Button 2";
  state->btns[1].result = &res2;
  button(app, window_bounds, &state->btns[1]);

  op_offset(&app->oplist, (point_t){100, 0});

  bool res3 = false;
  state->btns[2] = button_template;
  state->btns[2].label = "Button 3";
  state->btns[2].result = &res3;
  button(app, window_bounds, &state->btns[2]);

  /*widget_draw(app, window_bounds, 
    &(widget_t){
      .draw = (widget_draw_t) text,
      .data = &(text_t){
        .color = (color_t){0, 0, 0, 255},
        .content = "Test",
        .font = &state->fontNormal,
        .size = 30,
      }
    }
  );*/
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
      font = application_register_font(app, input_pattern, fontFile, heightFactor, heightOffset);
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

  state.fontNormal = load_font(app, "Roboto", 0.685, 0.115);
  //state.fontNormal = register_font(app, "sans", "/home/pit/code/nanovg-test/nanovg/example/Roboto-Regular.ttf", 0.71, 0.11);

  application_loop(app, (AppLoopFunction) draw, &state);
  application_free(app);
}

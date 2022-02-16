#include "application.h"
#include "nanovg/src/nanovg.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <fontconfig/fontconfig.h>
#include "events/events.h"
#include "widgets/widgets.h"
#include "element/element.h"
#include "font/font.h"

typedef struct {
  Font fontNormal;
  char text[200];
  int textlen;
  int fontSize;
} State;

void draw(AppContext *app, State *state) {
  bbox_t window_bounds = bbox_from_dims((point_t){0,0}, app->window.width, app->window.height);
  widget_draw(app, window_bounds,
    row(
      .spacing = 20,
      .item_count = 2,
      .items = (element_t[]){
        {
          .widget = rect(.color = nvgRGB(200, 100, 50)),
          .width  = {15, unit_percent},
          .height = {100, unit_percent},
          .x_align = align_start,
          .y_align = align_end,
        },
        {
          .widget = rect(.color = nvgRGB(200, 100, 50)),
          .width  = {100, unit_px},
          .height = {100, unit_px},
          .x_align = align_start,
          .y_align = align_end,
        },
      }
    )
  );
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

  state.fontNormal = load_font(app, "cmr10", 0.685, 0.115);
  //state.fontNormal = register_font(app, "sans", "/home/pit/code/nanovg-test/nanovg/example/Roboto-Regular.ttf", 0.71, 0.11);

  application_loop(app, (AppLoopFunction) draw, &state);
  application_free(app);
}

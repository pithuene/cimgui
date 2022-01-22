#ifndef _WIDGETS_H
#define _WIDGETS_H

#include <stdbool.h>
#include "../utils/utils.h"
#include "../application.h"
#include "../nanovg/src/nanovg.h"

typedef struct {
  float x;
  float y;
  const char *label;
  NVGcolor background;
  NVGcolor background_hover;
  NVGcolor background_down;
} ButtonConfig;

bool button(AppContext *app, ButtonConfig conf);

#endif

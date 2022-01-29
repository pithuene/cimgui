#ifndef _WIDGETS_H
#define _WIDGETS_H

#include <stdbool.h>
#include "../utils/utils.h"
#include "../application.h"
#include "../nanovg/src/nanovg.h"

typedef DPoint(*WidgetDraw)(AppContext *, void *);

typedef struct {
  WidgetDraw draw;
  void      *data;
} Widget;

DPoint draw_widget(AppContext *context, Widget widget);

typedef struct {
  bool *result;
  float x;
  float y;
  const char *label;
  NVGcolor background;
  NVGcolor background_hover;
  NVGcolor background_down;
} ButtonConfig;

DPoint button_draw(AppContext *context, ButtonConfig *conf);

Widget button(ButtonConfig *conf);

#endif

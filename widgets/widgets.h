#ifndef _WIDGETS_H
#define _WIDGETS_H

#include <stdbool.h>
#include "../utils/utils.h"
#include "../application.h"
#include "../nanovg/src/nanovg.h"
#include "../font/font.h"

typedef DPoint(*WidgetDraw)(AppContext *, void *);
typedef DPoint(*WidgetSize)(AppContext *, void *);

typedef struct {
  WidgetDraw draw;
  WidgetSize size;
} Widget;

DPoint draw_widget(AppContext *context, Widget widget);

typedef struct {
  float x;
  float y;
  float w;
  float h;
  NVGcolor color;
} RectConfig;

extern Widget widg_rect;
DPoint rect_draw(AppContext *context, RectConfig *conf);
DPoint rect_size(AppContext *context, RectConfig *conf);

typedef struct {
  Font       *font;
  float       size;
  DPoint      position;
  const char *content;
  NVGcolor    color;
} TextConfig;

extern Widget widg_text;
DPoint text_draw(AppContext *context, TextConfig *conf);
DPoint text_size(AppContext *context, TextConfig *conf);

typedef struct {
  bool *result;
  float x;
  float y;
  const char *label;
  Font *label_font;
  float label_font_size;
  NVGcolor label_color;
  NVGcolor background;
  NVGcolor background_hover;
  NVGcolor background_down;
} ButtonConfig;

extern Widget widg_button;
DPoint button_draw(AppContext *context, ButtonConfig *conf);
DPoint button_size(AppContext *context, ButtonConfig *conf);

#endif

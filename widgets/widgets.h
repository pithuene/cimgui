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
  // Upper left corner of the widget
  DPoint position;
} widget_t;

DPoint widget_draw(AppContext *context, widget_t *widget);
DPoint widget_getsize(AppContext *context, widget_t *widget);

typedef struct {
  widget_t widget;
  float w;
  float h;
  NVGcolor color;
} rect_t;

DPoint rect_draw(AppContext *app, rect_t *conf);
DPoint rect_size(AppContext *app, rect_t *conf);

#define rect(...) \
  (widget_t*)&(rect_t){ \
    .widget.draw = (WidgetDraw) rect_draw, \
    .widget.size = (WidgetSize) rect_size, \
    __VA_ARGS__ \
  }

typedef struct {
  widget_t widget;
  float radius;
  NVGcolor color;
} circle_t;

DPoint circle_draw(AppContext *app, circle_t *conf);
DPoint circle_size(AppContext *app, circle_t *conf);
// Helper to cet circle center position
DPoint circle_center_at(DPoint center, float radius);

#define circle(...) \
  (widget_t*)&(circle_t){ \
    .widget.draw = (WidgetDraw) circle_draw, \
    .widget.size = (WidgetSize) circle_size, \
    __VA_ARGS__ \
  }

typedef struct {
  widget_t widget;
  Font       *font;
  float       size;
  const char *content;
  NVGcolor    color;
} text_t;

DPoint text_draw(AppContext *app, text_t *conf);
DPoint text_size(AppContext *app, text_t *conf);

#define text(...) \
  (widget_t*)&(text_t){ \
    .widget.draw = (WidgetDraw) text_draw, \
    .widget.size = (WidgetSize) text_size, \
    .color = nvgRGB(0,0,0), \
    __VA_ARGS__ \
  }

typedef struct {
  widget_t widget;
  bool *result;
  const char *label;
  Font *label_font;
  float label_font_size;
  NVGcolor label_color;
  NVGcolor background;
  NVGcolor background_hover;
  NVGcolor background_down;
} button_t;

DPoint button_draw(AppContext *app, button_t *conf);
DPoint button_size(AppContext *app, button_t *conf);

#define button(...) \
  (widget_t*)&(button_t){ \
    .widget.draw = (WidgetDraw) button_draw, \
    .widget.size = (WidgetSize) button_size, \
    __VA_ARGS__ \
  }

typedef struct {
  widget_t widget;
  float spacing;
  int item_count;
  widget_t **items;
} row_t;

DPoint row_draw(AppContext *app, row_t *conf);
DPoint row_size(AppContext *app, row_t *conf);

#define row(...) \
  (widget_t*)&(row_t){ \
    .widget.draw = (WidgetDraw) row_draw, \
    .widget.size = (WidgetSize) row_size, \
    __VA_ARGS__ \
  }

typedef struct {
  widget_t widget;
  Font    *font;
  int     *value;
  int      min;
  int      max;
  int      step;
} slider_t;

DPoint slider_draw(AppContext *app, slider_t *conf);
DPoint slider_size(AppContext *app, slider_t *conf);

#define slider(...) \
  (widget_t*)&(slider_t){ \
    .widget.draw = (WidgetDraw) slider_draw, \
    .widget.size = (WidgetSize) slider_size, \
    .min = 0, \
    .min = 100, \
    .step = 1, \
    __VA_ARGS__ \
  }

#endif

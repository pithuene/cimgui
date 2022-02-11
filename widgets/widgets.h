#ifndef _WIDGETS_H
#define _WIDGETS_H

#include <stdbool.h>
#include "../application.h"
#include "../nanovg/src/nanovg.h"
#include "../font/font.h"

// Draw a widget into some constraints given as a bounding box.
// The widget must be drawn into the upper left corner of the constraints.
// Returns the actual size of the drawn widget.
typedef point_t(*WidgetDraw)(AppContext *, bbox_t, void *);
// Calculate the size of a widget without actually drawing it.
typedef point_t(*WidgetSize)(AppContext *, bbox_t, void *);

typedef struct {
  WidgetDraw draw;
  WidgetSize size;
} widget_t;

#define CONSTRAINT_NONE ((bbox_t){ .min = {0, 0}, .max = {999999, 999999}})

point_t widget_draw(AppContext *context, bbox_t constraints, widget_t *widget);
point_t widget_getsize(AppContext *context, bbox_t constraints, widget_t *widget);

typedef struct {
  widget_t widget;
  NVGcolor color;
} rect_t;

point_t rect_draw(AppContext *app, bbox_t constraints, rect_t *conf);
point_t rect_size(AppContext *app, bbox_t constraints, rect_t *conf);

#define rect(...) \
  (widget_t*)&(rect_t){ \
    .widget.draw = (WidgetDraw) rect_draw, \
    .widget.size = (WidgetSize) rect_size, \
    __VA_ARGS__ \
  }

typedef struct {
  widget_t widget;
  NVGcolor color;
} circle_t;

point_t circle_draw(AppContext *app, bbox_t constraints, circle_t *conf);
point_t circle_size(AppContext *app, bbox_t constraints, circle_t *conf);
// Helper to generate bbox for a circle
bbox_t circle_center_at(point_t center, float radius);

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

point_t text_draw(AppContext *app, bbox_t constraints, text_t *conf);
point_t text_size(AppContext *app, bbox_t constraints, text_t *conf);

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

point_t button_draw(AppContext *app, bbox_t constraints, button_t *conf);
point_t button_size(AppContext *app, bbox_t constraints, button_t *conf);

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
} stack_t;

point_t stack_draw(AppContext *app, bbox_t constraints, stack_t *conf);
point_t stack_size(AppContext *app, bbox_t constraints, stack_t *conf);

#define stack(...) \
  (widget_t*)&(stack_t){ \
    .widget.draw = (WidgetDraw) stack_draw, \
    .widget.size = (WidgetSize) stack_size, \
    __VA_ARGS__ \
  }

typedef struct {
  widget_t widget;
  float spacing;
  int item_count;
  widget_t **items;
} row_t;

point_t row_draw(AppContext *app, bbox_t constraints, row_t *conf);
point_t row_size(AppContext *app, bbox_t constraints, row_t *conf);

#define row(...) \
  (widget_t*)&(row_t){ \
    .widget.draw = (WidgetDraw) row_draw, \
    .widget.size = (WidgetSize) row_size, \
    __VA_ARGS__ \
  }

typedef struct {
  widget_t widget;
  widget_t *child;
} center_t;

point_t center_draw(AppContext *app, bbox_t constraints,center_t  *conf);
point_t center_size(AppContext *app, bbox_t constraints, center_t *conf);

#define center(...) \
  (widget_t*)&(center_t){ \
    .widget.draw = (WidgetDraw) center_draw, \
    .widget.size = (WidgetSize) center_size, \
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

point_t slider_draw(AppContext *app, bbox_t constraints, slider_t *conf);
point_t slider_size(AppContext *app, bbox_t constraints, slider_t *conf);

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

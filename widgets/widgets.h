#ifndef _WIDGETS_H
#define _WIDGETS_H

#include <stdbool.h>
#include "../application.h"
#include "../font/font.h"

typedef point_t(*widget_draw_t)(AppContext *, bbox_t, void *);

typedef struct {
  widget_draw_t draw;
  void         *data;
} widget_t;

#define CONSTRAINT_NONE ((bbox_t){ .min = {0, 0}, .max = {999999, 999999}})

point_t widget_draw(AppContext *context, bbox_t constraints, widget_t *widget);

typedef struct {
  oplist_t ops;
  point_t  dimensions;
} deferred_draw_t;

// Draw widget into seperate oplist.
deferred_draw_t widget_draw_deferred(AppContext *context, bbox_t constraints, widget_t *widget);

// Append the operations from a deferred draw to the appliaction oplist.
// Can be called multiple times to perform the same draw at multiple positions.
point_t deferred_draw_execute(AppContext *app, deferred_draw_t draw);

typedef struct {
  color_t color;
} rect_t;

point_t rect(AppContext *app, bbox_t constraints, rect_t *conf);

typedef struct {
  color_t color;
} circle_t;

point_t circle(AppContext *app, bbox_t constraints, circle_t *conf);

// Calculates the offset needed to put a circles center at given offset
point_t circle_center_at(point_t center, float radius);

// Calculate the dimensions for a circle of a given radius
bbox_t circle_dimensions(float radius);

typedef struct {
  Font       *font;
  float       size;
  const char *content;
  color_t     color;
} text_t;

point_t text(AppContext *app, bbox_t constraints, text_t *conf);

typedef struct {
  bool *result;
  const char *label;
  Font *label_font;
  float label_font_size;
  color_t label_color;
  color_t background;
  color_t background_hover;
  color_t background_down;
} button_t;

point_t button(AppContext *app, bbox_t constraints, button_t *conf);

/*
typedef struct {
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
  widget_t *child;
} center_t;

point_t center_draw(AppContext *app, bbox_t constraints,center_t  *conf);
point_t center_size(AppContext *app, bbox_t constraints, center_t *conf);

#define center(...) \
  (widget_t*)&(center_t){ \
    .widget.draw = (WidgetDraw) center_draw, \
    .widget.size = (WidgetSize) center_size, \
    __VA_ARGS__ \
  }*/

typedef struct {
  Font    *font;
  int     *value;
  int      min;
  int      max;
  int      step;
} slider_t;

point_t slider(AppContext *app, bbox_t constraints, slider_t *conf);

#endif

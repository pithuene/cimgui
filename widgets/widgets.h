#ifndef _WIDGETS_H
#define _WIDGETS_H

#include <stdbool.h>
#include "../application.h"
#include "../font/font.h"

typedef point_t(*widget_draw_t)(AppContext *, point_t, void *);

typedef struct {
  widget_draw_t draw;
  void         *data;
} widget_t;

#define widget(WIDGET, ...) \
  (widget_t){ \
    (widget_draw_t) WIDGET, \
    __VA_ARGS__ \
  }

#define CONSTRAINT_NONE ((bbox_t){ .min = {0, 0}, .max = {999999, 999999}})

point_t widget_draw(AppContext *context, point_t constraints, widget_t widget);

typedef struct {
  oplist_t ops;
  point_t  dimensions;
} deferred_draw_t;

// Draw widget into seperate oplist.
deferred_draw_t widget_draw_deferred(AppContext *context, point_t constraints, widget_t widget);

// Append the operations from a deferred draw to the appliaction oplist.
// Can be called multiple times to perform the same draw at multiple positions.
point_t deferred_draw_execute(AppContext *app, deferred_draw_t draw);

typedef struct {
  color_t color;
} rect_t;

point_t rect(AppContext *app, point_t constraints, rect_t *conf);

typedef struct {
  color_t color;
} circle_t;

point_t circle(AppContext *app, point_t constraints, circle_t *conf);

// Calculates the offset needed to put a circles center at given offset
point_t circle_center_at(point_t center, float radius);

// Calculate the dimensions for a circle of a given radius
point_t circle_dimensions(float radius);

typedef struct {
  Font       *font;
  float       size;
  const char *content;
  const char *content_end;
  color_t     color;
} text_t;

point_t text(AppContext *app, point_t constraints, text_t *conf);

typedef struct {
  Font         *font;
  float         size;
  const rune_t *content;
  const rune_t *content_end;
  color_t       color;
} rune_text_t;

point_t rune_text(AppContext *app, point_t constraints, rune_text_t *conf);

typedef struct {
  Font       *font;
  float       size;
  const char *content;
  const char *content_end;
  color_t     color;
  int16_t     max_rows;
  float       spacing;
} paragraph_t;

// Layout text over multiple lines.
// Spacing is the pixel width between lines.
point_t paragraph(AppContext *app, point_t constraints, paragraph_t *conf);

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

point_t button(AppContext *app, point_t constraints, button_t *conf);

typedef struct {
  Font    *font;
  int     *value;
  int      min;
  int      max;
  int      step;
} slider_t;

point_t slider(AppContext *app, point_t constraints, slider_t *conf);

#endif

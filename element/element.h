#ifndef _ELEMENT_H
#define _ELEMENT_H

#include "../widgets/widgets.h"

typedef enum {
  unit_px,
  unit_percent,
  // TODO: flex grow factor?
} size_unit_t;

typedef struct {
  double      size;
  size_unit_t unit;
} unit_length_t;

double unit_length_in_px(unit_length_t length, double container_length);

typedef enum {
  align_start,
  align_center,
  align_end,
} axis_alignment_t;

typedef struct {
  widget_t        *widget;
  unit_length_t    width;
  unit_length_t    height;
  axis_alignment_t x_align;
  axis_alignment_t y_align;
  // TODO: padding?
} element_t;

typedef struct {
  widget_t widget;
  float spacing;
  int item_count;
  element_t *items;
} row_t;

point_t row_draw(AppContext *app, bbox_t constraints, row_t *conf);
point_t row_size(AppContext *app, bbox_t constraints, row_t *conf);

#define row(...) \
  (widget_t*)&(row_t){ \
    .widget.draw = (WidgetDraw) row_draw, \
    .widget.size = (WidgetSize) row_size, \
    __VA_ARGS__ \
  }

point_t column_draw(AppContext *app, bbox_t constraints, row_t *conf);
point_t column_size(AppContext *app, bbox_t constraints, row_t *conf);

#define column(...) \
  (widget_t*)&(row_t){ \
    .widget.draw = (WidgetDraw) column_draw, \
    .widget.size = (WidgetSize) column_size, \
    __VA_ARGS__ \
  }

#endif

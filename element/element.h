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
  float spacing;
  int item_count;
  element_t *items;
} row_t;

point_t row(AppContext *app, point_t constraints, row_t *conf);
point_t column(AppContext *app, point_t constraints, row_t *conf);


#endif

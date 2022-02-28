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
  // TODO: This should not be a pointer.
  widget_t        *widget;
  unit_length_t    width;
  unit_length_t    height;
  axis_alignment_t x_align;
  axis_alignment_t y_align;
  // TODO: padding?
} element_t;

typedef struct {
  size_t     count;
  element_t *elements;
} element_children_t;

typedef struct {
  float              spacing;
  element_children_t children;
} row_t;

typedef row_t column_t;

point_t row(AppContext *app, point_t constraints, row_t *conf);
point_t column(AppContext *app, point_t constraints, column_t *conf);

#define element_children(...) \
  (element_children_t){ \
    .count = (sizeof((element_t[]){ __VA_ARGS__ })/sizeof(element_t)), \
    .elements = (element_t[]) { \
      __VA_ARGS__ \
    } \
  } \

#endif

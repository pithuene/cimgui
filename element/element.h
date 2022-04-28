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
  widget_t         widget;
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

typedef enum {
  overflow_handling_none,
  overflow_handling_scroll,
} overflow_handling_t;

typedef struct {
  float               spacing;
  overflow_handling_t overflow_handling;
  float               scroll_offset;
  element_children_t  children;
} row_t;

typedef row_t column_t;

point_t row(AppContext *app, point_t constraints, row_t *conf);
point_t column(AppContext *app, point_t constraints, column_t *conf);

typedef struct {
  widget_t widget;
  bool     visible;
} scroll_item_t;

typedef struct {
  widget_t *children;
  int child_count;
  float spacing;
  // The index of the first visible element.
  // Rendering starts here, and renders the following children until the constraints are filled.
  int first_shown_index;
  // This is set during rendering to the index of the last rendered child.
  // Can be used to figure out which children are visible and which aren't.
  int last_shown_index;
  // The number of pixels of the element at the top of first_shown_index, which are not shown
  float scroll_offset;
} scrollable_column_t;

point_t scrollable_column(AppContext *app, point_t constraints, scrollable_column_t *conf);

#define element_children(...) \
  (element_children_t){ \
    .count = (sizeof((element_t[]){ __VA_ARGS__ })/sizeof(element_t)), \
    .elements = (element_t[]) { \
      __VA_ARGS__ \
    } \
  } \

#endif

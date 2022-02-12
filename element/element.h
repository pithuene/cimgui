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

// TODO: Rename to just row and remove the other row
typedef struct {
  widget_t widget;
  float spacing;
  int item_count;
  element_t *items;
} elem_row_t;

point_t elem_row_draw(AppContext *app, bbox_t constraints, elem_row_t *conf);
point_t elem_row_size(AppContext *app, bbox_t constraints, elem_row_t *conf);

#define elem_row(...) \
  (widget_t*)&(elem_row_t){ \
    .widget.draw = (WidgetDraw) elem_row_draw, \
    .widget.size = (WidgetSize) elem_row_size, \
    __VA_ARGS__ \
  }

// Use an element as a widget
// TODO: Do I even need this?
/*typedef struct {
  widget_t   widget;
  element_t *element;
} element_widget_t;

point_t element_draw(AppContext *app, bbox_t constraints, rect_t *conf);
point_t element_size(AppContext *app, bbox_t constraints, rect_t *conf);

#define element_to_widget(ELEMENT) \
  (widget_t*)&(element_widget_t){ \
    .widget.draw = (WidgetDraw) rect_draw, \
    .widget.size = (WidgetSize) rect_size, \
    .element     = ELEMENT \
  }*/

#endif

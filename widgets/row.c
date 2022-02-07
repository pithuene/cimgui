#include "widgets.h"

point_t row_draw(AppContext *app, bbox_t constraints, row_t *conf) {
  float total_width  = 0;
  float total_height = 0;
  float target_size_per_child = bbox_width(constraints) / (float) conf->item_count;

  for (int i = 0; i < conf->item_count; i++) {
    bbox_t child_constraints = {
      .min = {
        .x = constraints.min.x + total_width,
        .y = constraints.min.y,
      },
      .max = {
        .x = constraints.min.x + total_width + target_size_per_child,
        .y = constraints.max.y,
      },
    };
    point_t childsize = widget_draw(app, child_constraints, conf->items[i]);
    total_width += childsize.x;
    if (i < conf->item_count - 1) total_width += conf->spacing;
    if (childsize.y > total_height) total_height = childsize.y;
  }

  return (point_t){
    .x = total_width,
    .y = total_height,
  };
}

point_t row_size(AppContext *app, bbox_t constraints, row_t *conf) {
  float total_width  = 0;
  float total_height = 0;

  for (int i = 0; i < conf->item_count; i++) {
    bbox_t child_constraints = {
      .min = {
        .x = constraints.min.x + total_width,
        .y = constraints.min.y,
      },
      .max = constraints.max,
    };
    point_t childsize = widget_getsize(app, child_constraints, conf->items[i]);
    total_width += childsize.x;
    if (i < conf->item_count - 1) total_width += conf->spacing;
    if (childsize.y > total_height) total_height = childsize.y;
  }

  return (point_t){
    .x = total_width,
    .y = total_height,
  };
}


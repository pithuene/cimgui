#include "widgets.h"

point_t stack_draw(AppContext *app, bbox_t constraints, stack_t *conf) {
  point_t max_dimensions = (point_t){
    .x = 0,
    .y = 0,
  };

  for (int i = 0; i < conf->item_count; i++) {
    point_t childsize = widget_draw(app, constraints, conf->items[i]);
    if (childsize.x > max_dimensions.x) max_dimensions.x = childsize.x;
    if (childsize.y > max_dimensions.y) max_dimensions.y = childsize.y;
  }

  return max_dimensions;
}

point_t stack_size(AppContext *app, bbox_t constraints, stack_t *conf) {
  point_t max_dimensions = (point_t){
    .x = 0,
    .y = 0,
  };

  for (int i = 0; i < conf->item_count; i++) {
    point_t childsize = widget_getsize(app, constraints, conf->items[i]);
    if (childsize.x > max_dimensions.x) max_dimensions.x = childsize.x;
    if (childsize.y > max_dimensions.y) max_dimensions.y = childsize.y;
  }

  return max_dimensions;
}


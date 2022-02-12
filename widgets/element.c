#include "widgets.h"

point_t center_draw(AppContext *app, bbox_t constraints, center_t *conf) {
  point_t child_size = widget_getsize(app, constraints, conf->child);
  point_t offset = {
    .x = (bbox_width(constraints)  - child_size.x) / 2,
    .y = (bbox_height(constraints) - child_size.y) / 2,
  };
  bbox_t offset_constraints = bbox_move(constraints, offset);
  widget_draw(app, offset_constraints, conf->child);

  return bbox_dims(constraints);
}

point_t center_size(AppContext *app, bbox_t constraints, center_t *conf) {
  return bbox_dims(constraints);
}


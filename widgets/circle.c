#include "widgets.h"

bbox_t circle_dimensions(float radius) {
  return (bbox_t){
    .min = {0,0},
    .max = {2*radius,2*radius},
  };
}

point_t circle_center_at(point_t center, float radius) {
  return (point_t){
    .x = center.x - radius,
    .y = center.y - radius,
  };
}

point_t circle(AppContext *app, bbox_t constraints, circle_t *conf) {
  // Diameter is the minimum of width and height
  float diameter = bbox_width(constraints);
  if (bbox_height(constraints) < diameter)
    diameter = bbox_height(constraints);

  float radius = diameter / 2.0;

  op_begin_path(&app->oplist);
  op_circle(&app->oplist, radius);
  op_fill_color(&app->oplist, conf->color);
  op_fill(&app->oplist);

  return (point_t){
    .x = diameter,
    .y = diameter,
  };
}

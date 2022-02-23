#include "widgets.h"

point_t circle_dimensions(float radius) {
  return (point_t){2*radius,2*radius};
}

point_t circle_center_at(point_t center, float radius) {
  return (point_t){
    .x = center.x - radius,
    .y = center.y - radius,
  };
}

point_t circle(AppContext *app, point_t constraints, circle_t *conf) {
  // Diameter is the minimum of width and height
  float diameter = constraints.x;
  if (constraints.y < diameter)
    diameter = constraints.y;

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

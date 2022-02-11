#include "widgets.h"

bbox_t circle_center_at(point_t center, float radius) {
  return (bbox_t){
    .min = { center.x - radius, center.y - radius },
    .max = { center.x + radius, center.y + radius },
  };
}

point_t circle_draw(AppContext *app, bbox_t constraints, circle_t *conf) {
  // Diameter is the minimum of width and height
  float diameter = bbox_width(constraints);
  if (bbox_height(constraints) < diameter)
    diameter = bbox_height(constraints);

  float radius = diameter / 2.0;

  nvgBeginPath(app->vg);
  nvgCircle(
      app->vg,
      constraints.min.x + radius,
      constraints.min.y + radius,
      radius);
  nvgFillColor(app->vg, conf->color);
  nvgFill(app->vg);

  return (point_t){
    .x = diameter,
    .y = diameter,
  };
}

point_t circle_size(AppContext *app, bbox_t constraints, circle_t *conf) {
  // Diameter is the minimum of width and height
  float diameter = bbox_width(constraints);
  if (bbox_height(constraints) < diameter)
    diameter = bbox_height(constraints);

  return (point_t){
    .x = diameter,
    .y = diameter,
  };
}


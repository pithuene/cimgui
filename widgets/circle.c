#include "widgets.h"

point_t circle_center_at(point_t center, float radius) {
  return (point_t){
    .x = center.x - radius,
    .y = center.y - radius,
  };
}

point_t circle_draw(AppContext *app, circle_t *conf) {
  nvgBeginPath(app->vg);
  nvgCircle(
      app->vg,
      conf->widget.position.x + conf->radius,
      conf->widget.position.y + conf->radius,
      conf->radius);
  nvgFillColor(app->vg, conf->color);
  nvgFill(app->vg);

  return (point_t){
    .x = 2*conf->radius,
    .y = 2*conf->radius,
  };
}

point_t circle_size(AppContext *app, circle_t *conf) {
  return (point_t){
    .x = 2*conf->radius,
    .y = 2*conf->radius,
  };
}


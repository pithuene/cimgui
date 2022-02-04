#include "widgets.h"

DPoint circle_center_at(DPoint center, float radius) {
  return (DPoint){
    .x = center.x - radius,
    .y = center.y - radius,
  };
}

DPoint circle_draw(AppContext *app, circle_t *conf) {
  nvgBeginPath(app->vg);
  nvgCircle(
      app->vg,
      conf->widget.position.x + conf->radius,
      conf->widget.position.y + conf->radius,
      conf->radius);
  nvgFillColor(app->vg, conf->color);
  nvgFill(app->vg);

  return (DPoint){
    .x = 2*conf->radius,
    .y = 2*conf->radius,
  };
}

DPoint circle_size(AppContext *app, circle_t *conf) {
  return (DPoint){
    .x = 2*conf->radius,
    .y = 2*conf->radius,
  };
}


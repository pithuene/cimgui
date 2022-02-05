#include "widgets.h"

point_t rect_draw(AppContext *app, rect_t *conf) {
  nvgBeginPath(app->vg);
  nvgRect(app->vg, conf->widget.position.x, conf->widget.position.y, conf->w, conf->h);
  nvgFillColor(app->vg, conf->color);
  nvgFill(app->vg);

  return (point_t){
    .x = conf->w,
    .y = conf->h,
  };
}

point_t rect_size(AppContext *app, rect_t *conf) {
  return (point_t){
    .x = conf->w,
    .y = conf->h,
  };
}


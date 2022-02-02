#include "widgets.h"

DPoint rect_draw(AppContext *app, rect_t *conf) {
  nvgBeginPath(app->vg);
  nvgRect(app->vg, conf->x, conf->y, conf->w, conf->h);
  nvgFillColor(app->vg, conf->color);
  nvgFill(app->vg);

  return (DPoint){
    .x = conf->w,
    .y = conf->h,
  };
}

DPoint rect_size(AppContext *app, rect_t *conf) {
  return (DPoint){
    .x = conf->w,
    .y = conf->h,
  };
}


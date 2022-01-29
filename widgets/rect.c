#include "widgets.h"

DPoint rect_draw(AppContext *app, RectConfig *conf) {
  nvgBeginPath(app->vg);
  nvgRect(app->vg, conf->x, conf->y, conf->w, conf->h);
  nvgFillColor(app->vg, conf->color);
  nvgFill(app->vg);

  return (DPoint){
    .x = conf->w,
    .y = conf->h,
  };
}

DPoint rect_size(AppContext *app, RectConfig *conf) {
  return (DPoint){
    .x = conf->w,
    .y = conf->h,
  };
}

Widget widg_rect = {
  .draw = (WidgetDraw) rect_draw,
  .size = (WidgetSize) rect_size,
};

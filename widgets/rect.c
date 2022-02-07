#include "widgets.h"
#include <stdio.h>

point_t rect_draw(AppContext *app, bbox_t constraints, rect_t *conf) {
  nvgBeginPath(app->vg);
  nvgRect(app->vg,
      constraints.min.x,
      constraints.min.y,
      bbox_width(constraints),
      bbox_height(constraints));
  nvgFillColor(app->vg, conf->color);
  nvgFill(app->vg);

  return bbox_dims(constraints);
}

point_t rect_size(AppContext *app, bbox_t constraints, rect_t *conf) {
  return bbox_dims(constraints);
}


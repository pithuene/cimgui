#include "widgets.h"

point_t rect(AppContext *app, bbox_t constraints, rect_t *conf) {
  op_begin_path(&app->oplist);
  op_rect(
    &app->oplist,
    bbox_width(constraints),
    bbox_height(constraints)
  );
  op_fill_color(&app->oplist, conf->color);
  op_fill(&app->oplist);

  return bbox_dims(constraints);
}


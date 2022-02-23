#include "widgets.h"

point_t rect(AppContext *app, point_t constraints, rect_t *conf) {
  op_begin_path(&app->oplist);
  op_rect(
    &app->oplist,
    constraints.x,
    constraints.y
  );
  op_fill_color(&app->oplist, conf->color);
  op_fill(&app->oplist);

  return constraints;
}


#include "widgets.h"

point_t text(AppContext *app, point_t constraints, text_t *conf) {
  op_begin_path(&app->oplist);
  op_fill_color(&app->oplist, conf->color);

  point_t bounds = text_bounds(app->vg, conf->size, conf->font, conf->content, NULL);

  op_text(&app->oplist, conf->size, conf->font, conf->content, NULL);

  return bounds;
}


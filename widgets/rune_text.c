#include "widgets.h"

point_t rune_text(AppContext *app, point_t constraints, rune_text_t *conf) {
  op_begin_path(&app->oplist);
  op_fill_color(&app->oplist, conf->color);

  Font *font = conf->font;
  if (!font) {
    font = &app->font_fallback;
  }

  point_t bounds = rune_text_bounds(app->vg, conf->size, font, conf->content, conf->content_end);

  op_rune_text(&app->oplist, conf->size, font, conf->content, conf->content_end);

  return bounds;
}


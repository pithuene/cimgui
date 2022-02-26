#include "widgets.h"
#include "../element/element.h"

point_t paragraph(AppContext *app, point_t constraints, paragraph_t *conf) {
  Font *font = conf->font;
  if (!font) {
    font = &app->font_fallback;
  }

	NVGtextRow rows[conf->max_rows];
  nvgFontSize(app->vg, conf->size / font->heightFactor);
  nvgFontFaceId(app->vg, font->handle);
	nvgTextAlign(app->vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
  int row_count = nvgTextBreakLines(
    app->vg,
    conf->content,
    conf->content_end,
    constraints.x,
    rows,
    conf->max_rows
  );

  element_t text_elements[row_count];

  for (int j = 0; j < row_count; j++) {
    text_t *text_element = arenaalloc(&app->ops_arena, sizeof(text_t));
    *text_element = (text_t){
      .color = conf->color,
      .content = rows[j].start,
      .content_end = rows[j].end,
      .font = font,
      .size = conf->size,
    };

    widget_t *widget = arenaalloc(&app->ops_arena, sizeof(widget_t));
    *widget = (widget_t){
        (widget_draw_t) text,
        text_element,
    };

    text_elements[j] = (element_t){.widget = widget};
  }

  return column(app, constraints, &(row_t){
    .spacing  = conf->spacing,
    .children = (element_children_t){
      .count    = row_count,
      .elements = text_elements,
    }
  });
}


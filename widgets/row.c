#include "widgets.h"

point_t row_draw(AppContext *app, row_t *conf) {
  float total_width  = 0;
  float total_height = 0;

  for (int i = 0; i < conf->item_count; i++) {
    conf->items[i]->position = (point_t){
      .x = conf->widget.position.x + total_width,
      .y = conf->widget.position.y,
    };
    point_t childsize = widget_draw(app, conf->items[i]);
    total_width += childsize.x;
    if (i < conf->item_count - 1) total_width += conf->spacing;
    if (childsize.y > total_height) total_height = childsize.y;
  }

  return (point_t){
    .x = total_width,
    .y = total_height,
  };
}

point_t row_size(AppContext *app, row_t *conf) {
  float total_width  = 0;
  float total_height = 0;

  for (int i = 0; i < conf->item_count; i++) {
    point_t childsize = widget_getsize(app, conf->items[i]);
    total_width += childsize.x;
    if (i < conf->item_count - 1) total_width += conf->spacing;
    if (childsize.y > total_height) total_height = childsize.y;
  }

  return (point_t){
    .x = total_width,
    .y = total_height,
  };
}


#include "widgets.h"

DPoint row_draw(AppContext *app, row_t *conf) {
  float total_width  = 0;
  float total_height = 0;

  for (int i = 0; i < conf->item_count; i++) {
    conf->items[i]->position = (DPoint){
      .x = conf->widget.position.x + total_width,
      .y = conf->widget.position.y,
    };
    DPoint childsize = widget_draw(app, conf->items[i]);
    total_width += childsize.x;
    if (i < conf->item_count - 1) total_width += conf->spacing;
    if (childsize.y > total_height) total_height = childsize.y;
  }

  return (DPoint){
    .x = total_width,
    .y = total_height,
  };
}

DPoint row_size(AppContext *app, row_t *conf) {
  float total_width  = 0;
  float total_height = 0;

  for (int i = 0; i < conf->item_count; i++) {
    DPoint childsize = widget_getsize(app, conf->items[i]);
    total_width += childsize.x;
    if (i < conf->item_count - 1) total_width += conf->spacing;
    if (childsize.y > total_height) total_height = childsize.y;
  }

  return (DPoint){
    .x = total_width,
    .y = total_height,
  };
}


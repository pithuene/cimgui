#include "widgets.h"

point_t button(AppContext *app, bbox_t constraints, button_t *conf) {
  bool result = false;

  float xPadding = 10;
  float yPadding = 10;

  point_t label_size = text_bounds(app->vg, conf->label_font_size, conf->label_font, conf->label, NULL);

  static bbox_t button_bounds = {0};
  op_area_mapping(&app->oplist, (point_t){
    2*xPadding + label_size.x,
    2*yPadding + label_size.y,
  }, &button_bounds);


  bool hover = intersects_point_bbox(app->cursor, button_bounds);

  color_t background = hover ? conf->background_hover : conf->background;

  eventqueue_foreach(InputEvent event, app->eventqueue) {
    if (event.type == InputMouseButtonPressEvent) {
      const bool press_over_btn = intersects_point_bbox(event.instance.mousebuttonpress.cursor, button_bounds);
      if (press_over_btn) {
        background = conf->background_down;
      }
    } else if (event.type == InputMouseButtonHeldDownEvent) {
      bool press_over_btn = intersects_point_bbox(event.instance.mousebuttonhelddown.initialPress.cursor, button_bounds);
      if (press_over_btn) {
        background = conf->background_down;
      }
    } else if (event.type == InputMouseButtonReleaseEvent) {
      bool press_over_btn = intersects_point_bbox(event.instance.mousebuttonrelease.initialPress.cursor, button_bounds);
      bool release_over_btn = intersects_point_bbox(event.instance.mousebuttonrelease.cursor, button_bounds);
      if (press_over_btn && release_over_btn) {
        result = true;
      }
    }
  }

  rect(app, button_bounds, &(rect_t){
    .color = background,
  });

  op_offset(&app->oplist, (point_t){
    .x = xPadding,
    .y = yPadding,
  });

  op_begin_path(&app->oplist);
  op_fill_color(&app->oplist, conf->label_color);
  op_text(&app->oplist, conf->label_font_size, conf->label_font, conf->label, NULL);

  op_offset(&app->oplist, (point_t){
    .x = -xPadding,
    .y = -yPadding,
  });

  *conf->result = result;

  return bbox_dims(button_bounds);
}

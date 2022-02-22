#include "widgets.h"

point_t button(AppContext *app, bbox_t constraints, button_t *conf) {
  bool result = false;

  float xPadding = 10;
  float yPadding = 10;

  deferred_draw_t label_draw = widget_draw_deferred(app, constraints,
    &(widget_t){
      (widget_draw_t)text,
      &(text_t){
        .color   = conf->label_color,
        .font    = conf->label_font,
        .size    = conf->label_font_size,
        .content = conf->label,
      }
    }
  );

  point_t label_size = label_draw.dimensions;

  point_t button_dimensions = (point_t){
    2*xPadding + label_size.x,
    2*yPadding + label_size.y,
  };

  static bbox_t drawn_button_bounds = {0};
  op_area_mapping(&app->oplist, button_dimensions, &drawn_button_bounds);

  bool hover = intersects_point_bbox(app->cursor, drawn_button_bounds);

  color_t background = hover ? conf->background_hover : conf->background;

  eventqueue_foreach(InputEvent event, app->eventqueue) {
    if (event.type == InputMouseButtonPressEvent) {
      const bool press_over_btn = intersects_point_bbox(event.instance.mousebuttonpress.cursor, drawn_button_bounds);
      if (press_over_btn) {
        background = conf->background_down;
      }
    } else if (event.type == InputMouseButtonHeldDownEvent) {
      bool press_over_btn = intersects_point_bbox(event.instance.mousebuttonhelddown.initialPress.cursor, drawn_button_bounds);
      if (press_over_btn) {
        background = conf->background_down;
      }
    } else if (event.type == InputMouseButtonReleaseEvent) {
      bool press_over_btn = intersects_point_bbox(event.instance.mousebuttonrelease.initialPress.cursor, drawn_button_bounds);
      bool release_over_btn = intersects_point_bbox(event.instance.mousebuttonrelease.cursor, drawn_button_bounds);
      if (press_over_btn && release_over_btn) {
        result = true;
      }
    }
  }

  rect(app, (bbox_t){.max = button_dimensions}, &(rect_t){.color = background});

  op_offset(&app->oplist, (point_t){
    .x = xPadding,
    .y = yPadding,
  });

  deferred_draw_execute(app, label_draw);

  op_offset(&app->oplist, (point_t){
    .x = -xPadding,
    .y = -yPadding,
  });

  *conf->result = result;

  return bbox_dims((bbox_t){.max = button_dimensions});
}

#include "widgets.h"

point_t button_draw(AppContext *app, bbox_t constraints, button_t *conf) {
  bool result = false;

  float xPadding = 10;
  float yPadding = 10;
  
  widget_t *label = text(
    .font = conf->label_font,
    .size = conf->label_font_size,
    .content = conf->label,
    .color = conf->label_color,
  );

  point_t label_size = widget_getsize(app, constraints, label);

  bbox_t button_bounds = bbox_from_dims(
      (point_t){constraints.min.x, constraints.min.y},
      2*xPadding + label_size.x,
      2*yPadding + label_size.y
  );

  bool hover = intersects_point_bbox(app->cursor, button_bounds);

  NVGcolor background = hover ? conf->background_hover : conf->background;

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

  widget_draw(app, button_bounds, rect(
    .color = background,
  ));

  widget_draw(app, bbox_move(constraints, (point_t){xPadding, yPadding}), label);

  *conf->result = result;

  return bbox_dims(button_bounds);
}

point_t button_size(AppContext *app, bbox_t constraints, button_t *conf) {
  float xPadding = 10;
  float yPadding = 10;

  widget_t *label = text(
    .font = conf->label_font,
    .size = conf->label_font_size,
    .content = conf->label,
  );
  point_t label_size = widget_getsize(app, constraints, label);

  return (point_t){
    .x = 2*xPadding + label_size.x,
    .y = 2*yPadding + label_size.y,
  };
}


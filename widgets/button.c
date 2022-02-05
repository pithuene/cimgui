#include "widgets.h"

point_t button_draw(AppContext *app, button_t *conf) {
  bool result = false;

  float xPadding = 10;
  float yPadding = 10;
  
  widget_t *label = text(
    .widget.position = {
      .x = conf->widget.position.x + xPadding,
      .y = conf->widget.position.y + yPadding,
    },
    .font = conf->label_font,
    .size = conf->label_font_size,
    .content = conf->label,
    .color = conf->label_color,
  );

  point_t label_size = widget_getsize(app, label);

  bbox_t button_bounds = bbox_from_dims(
      (point_t){conf->widget.position.x, conf->widget.position.y},
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

  widget_draw(app, rect(
    .widget.position = conf->widget.position,
    .w = bbox_width(button_bounds),
    .h = bbox_height(button_bounds),
    .color = background,
  ));

  widget_draw(app, label);

  *conf->result = result;

  return bbox_dims(button_bounds);
}

point_t button_size(AppContext *app, button_t *conf) {
  float xPadding = 10;
  float yPadding = 10;

	nvgFontSize(app->vg, 20);
	nvgFontFace(app->vg, "sans");
	nvgTextAlign(app->vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
  
  float bounds[4];
	nvgTextBounds(app->vg, conf->widget.position.x + xPadding, conf->widget.position.y + yPadding, conf->label, NULL, bounds);
  bounds[0] -= xPadding;
  bounds[1] -= yPadding;
  bounds[2] += xPadding;
  bounds[3] += yPadding;

  return (point_t){
    .x = bounds[2] - bounds[0],
    .y = bounds[3] - bounds[1],
  };
}


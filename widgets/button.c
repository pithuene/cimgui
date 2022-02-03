#include "widgets.h"

DPoint button_draw(AppContext *app, button_t *conf) {
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

  DPoint label_size = widget_getsize(app, label);

  float min_x  = conf->widget.position.x;
  float min_y  = conf->widget.position.y;
  float width  = 2*xPadding + label_size.x;
  float height = 2*yPadding + label_size.y;

  bool hover = 
    intersects_point_rect(
      app->cursor.x, app->cursor.y,
      min_x,
      min_y,
      width,
      height
    );
  NVGcolor background = hover ? conf->background_hover : conf->background;
  eventqueue_foreach(InputEvent event, app->eventqueue) {
    if (event.type == InputMouseButtonPressEvent) {
      bool press_over_btn = 
        intersects_point_rect(
          event.instance.mousebuttonpress.cursor.x, event.instance.mousebuttonpress.cursor.y,
          min_x,
          min_y,
          width,
          height
        );
      if (press_over_btn) {
        background = conf->background_down;
      }
    } else if (event.type == InputMouseButtonHeldDownEvent) {
      bool press_over_btn = 
        intersects_point_rect(
          event.instance.mousebuttonhelddown.initialPress.cursor.x, event.instance.mousebuttonhelddown.initialPress.cursor.y,
          min_x,
          min_y,
          width,
          height
        );
      if (press_over_btn) {
        background = conf->background_down;
      }
    } else if (event.type == InputMouseButtonReleaseEvent) {
      bool press_over_btn = 
        intersects_point_rect(
          event.instance.mousebuttonrelease.initialPress.cursor.x, event.instance.mousebuttonrelease.initialPress.cursor.y,
          min_x,
          min_y,
          width,
          height
        );
      bool release_over_btn = 
        intersects_point_rect(
          event.instance.mousebuttonrelease.cursor.x, event.instance.mousebuttonrelease.cursor.y,
          min_x,
          min_y,
          width,
          height
        );
      if (press_over_btn && release_over_btn) {
        result = true;
      }
    }
  }

  widget_draw(app, rect(
    .widget.position = conf->widget.position,
    .w = width,
    .h = height,
    .color = background,
  ));

  widget_draw(app, label);

  *conf->result = result;

  return (DPoint){
    .x = width,
    .y = height,
  };
}

DPoint button_size(AppContext *app, button_t *conf) {
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

  return (DPoint){
    .x = bounds[2] - bounds[0],
    .y = bounds[3] - bounds[1],
  };
}


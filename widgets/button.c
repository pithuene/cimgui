#include "widgets.h"

DPoint button_draw(AppContext *app, ButtonConfig *conf) {
  bool result = false;

  float xPadding = 10;
  float yPadding = 10;

  TextConfig label = {
    .font = conf->label_font,
    .size = conf->label_font_size,
    .position = {
      .x = conf->x + xPadding,
      .y = conf->y + yPadding,
    },
    .content = conf->label,
    .color = conf->label_color,
  };

  DPoint label_size = widg_text.size(app, &label);

  float min_x  = conf->x;
  float min_y  = conf->y;
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

  RectConfig background_rect = {
    .x = min_x,
    .y = min_y,
    .w = width,
    .h = height,
    .color = background,
  };
  widg_rect.draw(app, &background_rect);

	//nvgFillColor(app->vg, nvgRGBA(0,0,0,255));
	//nvgText(app->vg, conf->x + xPadding, conf->y + yPadding, conf->label, NULL);
  widg_text.draw(app, &label);

  *conf->result = result;

  return (DPoint){
    .x = width,
    .y = height,
  };
}

DPoint button_size(AppContext *app, ButtonConfig *conf) {
  float xPadding = 10;
  float yPadding = 10;

	nvgFontSize(app->vg, 20);
	nvgFontFace(app->vg, "sans");
	nvgTextAlign(app->vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
  
  float bounds[4];
	nvgTextBounds(app->vg, conf->x + xPadding, conf->y + yPadding, conf->label, NULL, bounds);
  bounds[0] -= xPadding;
  bounds[1] -= yPadding;
  bounds[2] += xPadding;
  bounds[3] += yPadding;

  return (DPoint){
    .x = bounds[2] - bounds[0],
    .y = bounds[3] - bounds[1],
  };
}

Widget widg_button = {
  .draw = (WidgetDraw) button_draw,
  .size = (WidgetSize) button_size,
};

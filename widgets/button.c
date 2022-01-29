#include "widgets.h"

DPoint button_draw(AppContext *app, ButtonConfig *conf) {
  bool result = false;

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

  bool hover = 
    intersects_point_rect(
      app->cursor.x, app->cursor.y,
      bounds[0],
      bounds[1],
      bounds[2] - bounds[0],
      bounds[3] - bounds[1]
    );
  NVGcolor background = hover ? conf->background_hover : conf->background;
  eventqueue_foreach(InputEvent event, app->eventqueue) {
    if (event.type == InputMouseButtonPressEvent) {
      bool press_over_btn = 
        intersects_point_rect(
          event.instance.mousebuttonpress.cursor.x, event.instance.mousebuttonpress.cursor.y,
          bounds[0],
          bounds[1],
          bounds[2] - bounds[0],
          bounds[3] - bounds[1]
        );
      if (press_over_btn) {
        background = conf->background_down;
      }
    } else if (event.type == InputMouseButtonHeldDownEvent) {
      bool press_over_btn = 
        intersects_point_rect(
          event.instance.mousebuttonhelddown.initialPress.cursor.x, event.instance.mousebuttonhelddown.initialPress.cursor.y,
          bounds[0],
          bounds[1],
          bounds[2] - bounds[0],
          bounds[3] - bounds[1]
        );
      if (press_over_btn) {
        background = conf->background_down;
      }
    } else if (event.type == InputMouseButtonReleaseEvent) {
      bool press_over_btn = 
        intersects_point_rect(
          event.instance.mousebuttonrelease.initialPress.cursor.x, event.instance.mousebuttonrelease.initialPress.cursor.y,
          bounds[0],
          bounds[1],
          bounds[2] - bounds[0],
          bounds[3] - bounds[1]
        );
      bool release_over_btn = 
        intersects_point_rect(
          event.instance.mousebuttonrelease.cursor.x, event.instance.mousebuttonrelease.cursor.y,
          bounds[0],
          bounds[1],
          bounds[2] - bounds[0],
          bounds[3] - bounds[1]
        );
      if (press_over_btn && release_over_btn) {
        result = true;
      }
    }
  }

  RectConfig background_rect = {
    .x = bounds[0],
    .y = bounds[1],
    .w = bounds[2] - bounds[0],
    .h = bounds[3] - bounds[1],
    .color = background,
  };
  rect.draw(app, &background_rect);

	nvgFillColor(app->vg, nvgRGBA(0,0,0,255));
	nvgText(app->vg, conf->x + xPadding, conf->y + yPadding, conf->label, NULL);

  *conf->result = result;

  return (DPoint){
    .x = bounds[2] - bounds[0],
    .y = bounds[3] - bounds[1]
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
    .y = bounds[3] - bounds[1]
  };
}

Widget button = {
  .draw = (WidgetDraw) button_draw,
  .size = (WidgetSize) button_size,
};

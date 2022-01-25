#include "widgets.h"

bool button(AppContext *app, ButtonConfig conf) {
  bool result = false;

  float xPadding = 10;
  float yPadding = 10;

	nvgFontSize(app->vg, 20);
	nvgFontFace(app->vg, "sans");
	nvgTextAlign(app->vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
  
  float bounds[4];
	nvgTextBounds(app->vg, conf.x + xPadding, conf.y + yPadding, conf.label, NULL, bounds);
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
  NVGcolor background = hover ? conf.background_hover : conf.background;
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
        background = conf.background_down;
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
        background = conf.background_down;
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

  nvgBeginPath(app->vg);
  nvgRect(app->vg, bounds[0], bounds[1], bounds[2] - bounds[0], bounds[3] - bounds[1]);
  nvgFillColor(app->vg, background);
  nvgFill(app->vg);

	nvgFillColor(app->vg, nvgRGBA(0,0,0,255));
	nvgText(app->vg, conf.x + xPadding, conf.y + yPadding, conf.label, NULL);

  return result;
}


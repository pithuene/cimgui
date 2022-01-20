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

  NVGcolor background = conf.background;
  bool cursor_over_btn = 
    intersects_point_rect(
      app->cursor.x, app->cursor.y,
      bounds[0],
      bounds[1],
      bounds[2] - bounds[0],
      bounds[3] - bounds[1]
    );

  eventqueue_foreach(InputEvent event, app->eventqueue) {
    if (event.type == InputMouseButtonEvent) {
      if (event.instance.mousebutton.action == GLFW_PRESS && cursor_over_btn) {
        background = conf.background_active;
      } else {
        background = conf.background;
      }
      if (event.instance.mousebutton.action == GLFW_RELEASE && cursor_over_btn) {
        result = true;
        // TODO: Force a redraw on the next frame here.
        // TODO: Otherwise, if some state is drawn before it is modified by the button, it will not be displayed until another event occured.
        // TODO: Maybe I should even handle this globally:
        // TODO: At the end of a frame which actually handled some events, force a redraw on the next frame. If the frame was already forced, don't.
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


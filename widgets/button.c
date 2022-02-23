#include "widgets.h"
#include "../checktag/checktag.h"
#include "../application.h"
#include <stdio.h>

point_t button(AppContext *app, point_t constraints, button_t *conf) {
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

  // TODO: Expecting that the label is a string constant which is used only for this button. Should probably be changed to expecting an id in the config.
  uint64_t input_area_id = (uint64_t) conf->label;
  check_tag((void *) input_area_id);
  input_area_t input_area = register_input_area(app, button_dimensions, input_area_id);

  bool hover = is_cursor_in_input_area(app, input_area);

  color_t background = hover ? conf->background_hover : conf->background;

  eventqueue_foreach(InputEvent event, app->eventqueue) {
    if (event.type == eventtype_mousebuttonpress) {
      const bool press_over_btn = is_point_in_input_area(event.instance.mousebuttonpress.cursor, input_area);
      if (press_over_btn) {
        background = conf->background_down;
      }
    } else if (event.type == eventtype_mousebuttonhelddown) {
      bool press_over_btn = is_point_in_input_area(event.instance.mousebuttonhelddown.initialPress.cursor, input_area);
      if (press_over_btn) {
        background = conf->background_down;
      }
    } else if (event.type == eventtype_mousebuttonrelease) {
      bool press_over_btn = is_point_in_input_area(event.instance.mousebuttonrelease.initialPress.cursor, input_area);
      bool release_over_btn = is_point_in_input_area(event.instance.mousebuttonrelease.cursor, input_area);
      if (press_over_btn && release_over_btn) {
        result = true;
      }
    }
  }

  rect(app, button_dimensions, &(rect_t){.color = background});

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

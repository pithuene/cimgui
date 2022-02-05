#include "widgets.h"
#include "../utils/utils.h"
#include <math.h>
#include <stdio.h>

point_t slider_draw(AppContext *app, slider_t *conf) {
  NVGcolor track_color = nvgRGB(66, 66, 66);
  NVGcolor track_value_color = nvgRGB(150, 150, 150);
  NVGcolor track_value_color_active = nvgRGB(20, 192, 80);
  NVGcolor slider_color = nvgRGB(192, 192, 192);
  float slider_radius = 8;
  float slider_middle_radius = 3;
  float track_width = 200;
  float track_line_width = 2;

  bbox_t widget_bounds = bbox_from_dims(
      (point_t){conf->widget.position.x + slider_radius, conf->widget.position.y},
      track_width,
      2*slider_radius);

  const bool is_being_hovered = intersects_point_bbox(app->cursor, widget_bounds);
  bool is_active = is_being_hovered;
  bool is_being_dragged = false;

  eventqueue_foreach(InputEvent event, app->eventqueue) {
    if (event.type == InputMouseButtonPressEvent || 
        event.type == InputMouseButtonHeldDownEvent) {
      point_t relevant_click;
      if (event.type == InputMouseButtonPressEvent)
        relevant_click = event.instance.mousebuttonpress.cursor;
      if (event.type == InputMouseButtonHeldDownEvent)
        relevant_click = event.instance.mousebuttonhelddown.initialPress.cursor;

      is_being_dragged = intersects_point_bbox(relevant_click, widget_bounds);

      if (is_being_dragged) {
        is_active = true;
        float cursor_x = app->cursor.x;
        clamp_float(
            &cursor_x,
            conf->widget.position.x + slider_radius,
            conf->widget.position.x + slider_radius + track_width);

        // The relative position on the track, between 0 and 1
        const float slider_offset = (cursor_x - (conf->widget.position.x + slider_radius)) / track_width;

        int value = round((conf->max - conf->min) * slider_offset + conf->min);
        clamp_int(&value, conf->min, conf->max);
        *conf->value = value;
      }
    }
  }

  point_t track_position = {
    .x = conf->widget.position.x + slider_radius,
    .y = conf->widget.position.y + slider_radius - (track_line_width / 2),
  };

  point_t slider_center = {
    .x = track_position.x,
    .y = track_position.y + (track_line_width / 2),
  };

  const float value_range = (conf->max - conf->min);
  float slider_offset = (*conf->value - conf->min) / value_range;
  if (slider_offset < 0) slider_offset = 0;
  if (slider_offset > 1) slider_offset = 1;
  const float track_value_width = track_width * slider_offset;
  slider_center.x += track_value_width;

  // Track
  widget_draw(app, rect(
    .widget.position = track_position,
    .color = track_color,
    .h     = track_line_width,
    .w     = track_width,
  ));

  // Active track
  widget_draw(app, rect(
    .widget.position = track_position,
    .color = is_active ? track_value_color_active : track_value_color,
    .h     = track_line_width,
    .w     = track_value_width,
  ));

  // Draw slider
  if (is_active) {
    widget_draw(app, circle(
      .widget.position = circle_center_at(slider_center, slider_radius),
      .color           = slider_color,
      .radius          = slider_radius,
    ));
    widget_draw(app, circle(
      .widget.position = circle_center_at(slider_center, slider_middle_radius),
      .color           = is_being_dragged ? track_value_color_active : track_color,
      .radius          = slider_middle_radius,
    ));

    // Draw value label
    char valuelabel[10];
    sprintf(valuelabel, "%d", *conf->value);
    point_t value_label_position = circle_center_at(slider_center, slider_radius);
    value_label_position.y += 25;
    widget_draw(app, text(
      .widget.position = value_label_position,
      .font = conf->font,
      .size = 10,
      .content = valuelabel,
    ));
  }


  return (point_t){
    .x = track_width+2*slider_radius,
    .y = 2*slider_radius,
  };
}

point_t slider_size(AppContext *app, slider_t *conf) {
  float slider_radius = 10;
  return (point_t){
    .x = 200,
    .y = 2*slider_radius,
  };
}


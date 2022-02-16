#include "widgets.h"
#include "../utils/utils.h"
#include <math.h>
#include <stdio.h>

point_t slider(AppContext *app, bbox_t constraints, slider_t *conf) {
  color_t track_color = {66, 66, 66, 255};
  color_t track_value_color = {150, 150, 150, 255};
  color_t track_value_color_active = {20, 192, 80, 255};
  color_t slider_color = {192, 192, 192, 255};
  float slider_radius = 8;
  float slider_middle_radius = 3;
  float track_width = 200;
  float track_line_width = 2;

  bbox_t widget_bounds = bbox_from_dims(
      (point_t){constraints.min.x + slider_radius, constraints.min.y},
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
            constraints.min.x + slider_radius,
            constraints.min.x + slider_radius + track_width);

        // The relative position on the track, between 0 and 1
        const float slider_offset = (cursor_x - (constraints.min.x + slider_radius)) / track_width;

        int value = round((conf->max - conf->min) * slider_offset + conf->min);
        clamp_int(&value, conf->min, conf->max);
        *conf->value = value;
      } else {
        // If a drag is ongoing, but it didn't start over the widget (so something unrelated is being dragged)
        // Don't show the slider
        is_active = false;
      }
    }
  }

  point_t track_position = {
    .x = constraints.min.x + slider_radius,
    .y = constraints.min.y + slider_radius - (track_line_width / 2),
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
  rect(
    app,
    (bbox_t){
      .min = track_position,
      .max = point_add(track_position, (point_t){track_width, track_line_width}),
    },
    &(rect_t){.color = track_color}
  );

  // Active track
  rect(
    app,
    (bbox_t){
      .min = track_position,
      .max = point_add(track_position, (point_t){track_value_width, track_line_width}),
    },
    &(rect_t){.color = is_active ? track_value_color_active : track_value_color}
  );

  // Draw slider
  if (is_active) {
    // TODO: You need offset here
    circle(
      app,
      circle_center_at(slider_center, slider_radius),
      &(circle_t){.color = slider_color}
    );
    circle(
      app,
      circle_center_at(slider_center, slider_middle_radius),
      &(circle_t){.color = is_being_dragged ? track_value_color_active : track_color}
    );

    // Draw value label
    char valuelabel[10];
    sprintf(valuelabel, "%d", *conf->value);
    bbox_t value_label_position = circle_center_at(slider_center, slider_radius);
    value_label_position = bbox_move(value_label_position, (point_t){.y = 25});
    text(
      app,
      value_label_position,
      &(text_t){
        .font = conf->font,
        .size = 10,
        .content = valuelabel,
        .color = {0,0,0,255},
      }
    );
  }

  return (point_t){
    .x = track_width+2*slider_radius,
    .y = 2*slider_radius,
  };
}

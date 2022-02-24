#include "widgets.h"
#include "../utils/utils.h"
#include "../checktag/checktag.h"
#include <math.h>
#include <stdio.h>

point_t slider(AppContext *app, point_t constraints, slider_t *conf) {
  color_t track_color = {66, 66, 66, 255};
  color_t track_value_color = {150, 150, 150, 255};
  color_t track_value_color_active = {20, 192, 80, 255};
  color_t slider_color = {192, 192, 192, 255};
  float slider_radius = 8;
  float slider_middle_radius = 3;
  float track_width = 200;
  float track_line_width = 2;

  point_t widget_dimensions = (point_t){
    .x = track_width,
    .y = 2*slider_radius,
  };

  // Expecting that nothing else uses the slider value as an id.
  uint64_t input_area_id = (uint64_t) conf->value;
  check_tag((void *) input_area_id);
  input_area_t input_area = register_input_area(app, widget_dimensions, input_area_id);

  const bool is_being_hovered = is_cursor_in_input_area(app, input_area);
  bool is_active = is_being_hovered;
  bool is_being_dragged = false;

  eventqueue_foreach(InputEvent event, app->eventqueue) {
    if (event.type == eventtype_mousebuttonpress || 
        event.type == eventtype_mousebuttonhelddown) {
      point_t relevant_click;
      if (event.type == eventtype_mousebuttonpress)
        relevant_click = event.instance.mousebuttonpress.cursor;
      if (event.type == eventtype_mousebuttonhelddown)
        relevant_click = event.instance.mousebuttonhelddown.initialPress.cursor;

      is_being_dragged = is_point_in_input_area(relevant_click, input_area);

      if (is_being_dragged) {
        is_active = true;

        // X position relative to the left end of the track
        float cursor_x = position_relative_to_input_area(app->cursor, input_area).x;
        clamp_float(&cursor_x, 0, track_width);

        // The relative position on the track, between 0 and 1
        const float slider_offset = cursor_x / track_width;

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

  point_t slider_center = {
    .x = 0,
    .y = track_line_width / 2,
  };

  const float value_range = (conf->max - conf->min);
  float slider_offset = (*conf->value - conf->min) / value_range;
  if (slider_offset < 0) slider_offset = 0;
  if (slider_offset > 1) slider_offset = 1;
  const float track_value_width = track_width * slider_offset;
  slider_center.x += track_value_width;

  with_offset(&app->oplist, (point_t){.y = slider_radius - (track_line_width / 2)}) {
    // Track
    rect(
      app,
      (point_t){track_width, track_line_width},
      &(rect_t){.color = track_color}
    );

    // Active track
    rect(
      app,
      (point_t){track_value_width, track_line_width},
      &(rect_t){.color = is_active ? track_value_color_active : track_value_color}
    );

    // Draw slider
    if (is_active) {
      with_offset(&app->oplist, circle_center_at(slider_center, slider_radius)) {
        circle(
          app,
          circle_dimensions(slider_radius),
          &(circle_t){.color = slider_color}
        );
      }

      with_offset(&app->oplist, circle_center_at(slider_center, slider_middle_radius)) {
        circle(
          app,
          circle_dimensions(slider_middle_radius),
          &(circle_t){.color = is_being_dragged ? track_value_color_active : track_color}
        );
      }

      // Draw value label
      char *valuelabel = arenaalloc(&app->ops_arena, 10);
      sprintf(valuelabel, "%d", *conf->value);

      deferred_draw_t value_label_draw = widget_draw_deferred(app, constraints,
        &(widget_t){
          (widget_draw_t) text,
          &(text_t){
          .font = conf->font,
          .size = 10,
          .content = valuelabel,
          .color = {0,0,0,255},
          }
        }
      );

      point_t value_label_offset = point_add(slider_center, (point_t){
        .y = 15,
        .x = -value_label_draw.dimensions.x / 2
      });

      with_offset(&app->oplist, value_label_offset) {
        deferred_draw_execute(app, value_label_draw);
      }
    }
  }

  return (point_t){
    .x = track_width,
    .y = 2*slider_radius,
  };
}

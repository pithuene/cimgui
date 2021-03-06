#include "element.h"
#include <stdbool.h>
#include <stdio.h>

point_t column(AppContext *app, point_t constraints, column_t *conf) {
  float current_width  = 0;
  float max_height = 0;

  // Calculate the child sizes.
  // Non left-aligned childredn need to know the sizes of the children afterwards to determine their position.
  point_t child_target_sizes[conf->children.count];
  deferred_draw_t child_draws[conf->children.count];
  double total_widget_width = 0;
  double center_widget_width = -conf->spacing;
  axis_alignment_t current_alignment = align_start;
  for (int i = 0; i < conf->children.count; i++) {
    // Alignment push along
    if (conf->children.elements[i].y_align > current_alignment) current_alignment = conf->children.elements[i].y_align;

    child_target_sizes[i] = (point_t){
      .x = unit_length_in_px(conf->children.elements[i].width, constraints.x),
      .y = unit_length_in_px(conf->children.elements[i].height, constraints.y),
    };
    child_draws[i] = widget_draw_deferred(app, child_target_sizes[i], conf->children.elements[i].widget);

    total_widget_width += child_draws[i].dimensions.y;
    if (i < conf->children.count - 1) total_widget_width += conf->spacing;
    if (current_alignment == align_center)
      center_widget_width += child_draws[i].dimensions.y + conf->spacing;

    if (child_draws[i].dimensions.x > max_height) max_height = child_draws[i].dimensions.x;
  }

  // If constraints height is greater than the talles child, the row fills that height
  if (constraints.x > max_height) max_height = constraints.x;

  // If there are no centered children, set centered widget size to zero
  if (center_widget_width < 0) center_widget_width = 0;

  // Is the row wider than its constraints.
  // If so, there are no gaps.
  double total_width = total_widget_width;
  bool overflowing = true;
  if (total_width < constraints.y) {
    overflowing = false;
    total_width = constraints.y;
  }

  // Elements are layed out left to right.
  // As soon as an element has "higher" alignment (start -> center -> end),
  // all following elements are pushed along to atleast that alignment.
  current_alignment = align_start;
  double remaining_widget_width = total_widget_width;

  if (overflowing && conf->overflow_handling == overflow_handling_scroll) {
    // Scroll handling
    op_clip(&app->oplist, constraints.x, constraints.y);
    with_offset(&app->oplist, (point_t){0,-conf->scroll_offset}) {
      for (int i = 0; i < conf->children.count; i++) {
        point_t topleft = {
          .y = current_width,
          .x = 0, // align_start
        };
        if (conf->children.elements[i].x_align == align_center) {
          topleft.x = (max_height - child_draws[i].dimensions.x) / 2;
        } else if (conf->children.elements[i].x_align == align_end) {
          topleft.x = max_height - child_draws[i].dimensions.x;
        }

        with_offset(&app->oplist, topleft) {
          deferred_draw_execute(app, child_draws[i]);
        }

        current_width          += child_draws[i].dimensions.y;
        remaining_widget_width -= child_draws[i].dimensions.y;

        if (i < conf->children.count - 1) {
          current_width          += conf->spacing;
          remaining_widget_width -= conf->spacing;
        }
      }
    }
    op_reset_clip(&app->oplist);

    return (point_t){
      .y = constraints.y,
      .x = max_height,
    };
  } else {
    for (int i = 0; i < conf->children.count; i++) {
      // Alignment push along
      /*if (conf->children.elements[i].y_align > current_alignment) {
        current_alignment = conf->children.elements[i].y_align;
        if (current_alignment == align_center) {
          double center_start = (total_width - center_widget_width) / 2;
          if (current_width < center_start) {
            current_width = center_start;
          }
        }
        if (current_alignment == align_end) {
          current_width = total_width - remaining_widget_width;
        }
      }*/

      point_t topleft = {
        .y = current_width,
        .x = 0, // align_start
      };
      if (conf->children.elements[i].x_align == align_center) {
        topleft.x = (max_height - child_draws[i].dimensions.x) / 2;
      } else if (conf->children.elements[i].x_align == align_end) {
        topleft.x = max_height - child_draws[i].dimensions.x;
      }

      with_offset(&app->oplist, topleft) {
        deferred_draw_execute(app, child_draws[i]);
      }

      current_width          += child_draws[i].dimensions.y;
      remaining_widget_width -= child_draws[i].dimensions.y;

      if (i < conf->children.count - 1) {
        current_width          += conf->spacing;
        remaining_widget_width -= conf->spacing;
      }
    }

    return (point_t){
      .y = current_width,
      .x = max_height,
    };
  }
}

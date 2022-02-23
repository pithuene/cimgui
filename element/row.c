#include "element.h"
#include <stdio.h>

point_t row(AppContext *app, bbox_t constraints, row_t *conf) {
  float current_width  = 0;
  float max_height = 0;

  // Calculate the child sizes.
  // Non left-aligned childredn need to know the sizes of the children afterwards to determine their position.
  point_t child_target_sizes[conf->item_count];
  deferred_draw_t child_draws[conf->item_count];
  double total_widget_width = 0;
  double center_widget_width = -conf->spacing;
  axis_alignment_t current_alignment = align_start;
  for (int i = 0; i < conf->item_count; i++) {
    // Alignment push along
    if (conf->items[i].x_align > current_alignment) current_alignment = conf->items[i].x_align;

    child_target_sizes[i] = (point_t){
      .x = unit_length_in_px(conf->items[i].width, bbox_width(constraints)),
      .y = unit_length_in_px(conf->items[i].height, bbox_height(constraints)),
    };
    child_draws[i] = widget_draw_deferred(app, child_target_sizes[i], conf->items[i].widget);

    total_widget_width += child_draws[i].dimensions.x;
    if (i < conf->item_count - 1) total_widget_width += conf->spacing;
    if (current_alignment == align_center)
      center_widget_width += child_draws[i].dimensions.x + conf->spacing;

    if (child_draws[i].dimensions.y > max_height) max_height = child_draws[i].dimensions.y;
  }

  // If constraints height is greater than the talles child, the row fills that height
  if (bbox_height(constraints) > max_height) max_height = bbox_height(constraints);

  // If there are no centered children, set centered widget size to zero
  if (center_widget_width < 0) center_widget_width = 0;

  // Is the row wider than its constraints.
  // If so, there are no gaps.
  double total_width = total_widget_width;
  if (total_width < bbox_width(constraints)) {
    total_width = bbox_width(constraints);
  }

  // Elements are layed out left to right.
  // As soon as an element has "higher" alignment (start -> center -> end),
  // all following elements are pushed along to atleast that alignment.
  current_alignment = align_start;
  double remaining_widget_width = total_widget_width;

  for (int i = 0; i < conf->item_count; i++) {
    // Alignment push along
    if (conf->items[i].x_align > current_alignment) {
      current_alignment = conf->items[i].x_align;
      if (current_alignment == align_center) {
        double center_start = (total_width - center_widget_width) / 2;
        if (current_width < center_start) {
          current_width = center_start;
        }
      }
      if (current_alignment == align_end) {
        current_width = total_width - remaining_widget_width;
      }
    }

    point_t topleft = {
      .x = constraints.min.x + current_width,
      .y = constraints.min.y, // align_start
    };
    if (conf->items[i].y_align == align_center) {
      topleft.y = (max_height - child_draws[i].dimensions.y) / 2;
    } else if (conf->items[i].y_align == align_end) {
      topleft.y = max_height - child_draws[i].dimensions.y;
    }

    op_offset(&app->oplist, topleft);
    point_t childsize = deferred_draw_execute(app, child_draws[i]);
    op_offset(&app->oplist, point_multiply(topleft, -1));

    if (i < conf->item_count - 1) {
      current_width          += childsize.x;
      remaining_widget_width -= childsize.x;

      current_width          += conf->spacing;
      remaining_widget_width -= conf->spacing;
    }
  }

  return (point_t){
    .x = current_width,
    .y = max_height,
  };
}


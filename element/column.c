#include "element.h"
#include <stdio.h>

point_t elem_column_draw(AppContext *app, bbox_t constraints, elem_row_t *conf) {
  float current_width  = 0;
  float max_height = 0;

  // Calculate the child sizes.
  // Non left-aligned childredn need to know the sizes of the children afterwards to determine their position.
  point_t child_target_sizes[conf->item_count];
  point_t child_sizes[conf->item_count];
  double total_widget_width = 0;
  double center_widget_width = -conf->spacing;
  axis_alignment_t current_alignment = align_start;
  for (int i = 0; i < conf->item_count; i++) {
    // Alignment push along
    if (conf->items[i].y_align > current_alignment) current_alignment = conf->items[i].y_align;

    child_target_sizes[i] = (point_t){
      .x = unit_length_in_px(conf->items[i].width, bbox_width(constraints)),
      .y = unit_length_in_px(conf->items[i].height, bbox_height(constraints)),
    };
    child_sizes[i] = widget_getsize(app, (bbox_t){.max = child_target_sizes[i]}, conf->items[i].widget);

    total_widget_width += child_sizes[i].y;
    if (i < conf->item_count - 1) total_widget_width += conf->spacing;
    if (current_alignment == align_center)
      center_widget_width += child_sizes[i].y + conf->spacing;

    if (child_sizes[i].x > max_height) max_height = child_sizes[i].x;
  }

  // If constraints height is greater than the talles child, the row fills that height
  if (bbox_width(constraints) > max_height) max_height = bbox_width(constraints);

  // If there are no centered children, set centered widget size to zero
  if (center_widget_width < 0) center_widget_width = 0;

  // Is the row wider than its constraints.
  // If so, there are no gaps.
  double total_width = total_widget_width;
  if (total_width < bbox_height(constraints)) {
    total_width = bbox_height(constraints);
  }

  // Elements are layed out left to right.
  // As soon as an element has "higher" alignment (start -> center -> end),
  // all following elements are pushed along to atleast that alignment.
  current_alignment = align_start;
  double remaining_widget_width = total_widget_width;

  for (int i = 0; i < conf->item_count; i++) {
    // Alignment push along
    if (conf->items[i].y_align > current_alignment) {
      current_alignment = conf->items[i].y_align;
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
      .y = constraints.min.y + current_width,
      .x = constraints.min.x, // align_start
    };
    if (conf->items[i].x_align == align_center) {
      topleft.x = (max_height - child_sizes[i].x) / 2;
    } else if (conf->items[i].x_align == align_end) {
      topleft.x = max_height - child_sizes[i].x;
    }
    point_t bottomright = point_add(topleft, child_target_sizes[i]);
    bbox_t child_constraints = {.min = topleft, .max = bottomright};

    point_t childsize = widget_draw(app, child_constraints, conf->items[i].widget);

    if (i < conf->item_count - 1) {
      current_width          += childsize.y;
      remaining_widget_width -= childsize.y;

      current_width          += conf->spacing;
      remaining_widget_width -= conf->spacing;
    }
  }

  return (point_t){
    .y = current_width,
    .x = max_height,
  };
}

point_t elem_column_size(AppContext *app, bbox_t constraints, elem_row_t *conf) {
  float total_width  = 0;
  float total_height = 0;

  for (int i = 0; i < conf->item_count; i++) {
    point_t topleft = {
      .y = constraints.min.y + total_width,
      .x = constraints.min.x,
    };
    point_t target_child_size = {
      .x = unit_length_in_px(conf->items[i].width, bbox_width(constraints)),
      .y = unit_length_in_px(conf->items[i].height, bbox_height(constraints)),
    };
    point_t bottomright = point_add(topleft, target_child_size);
    bbox_t child_constraints = {.min = topleft, .max = bottomright};

    point_t childsize = widget_getsize(app, child_constraints, conf->items[i].widget);
    total_width += childsize.y;
    if (i < conf->item_count - 1) total_width += conf->spacing;
    if (childsize.x > total_height) total_height = childsize.x;
  }

  // If the row is not entirely full (gaps), the width is still the full constraint width.
  // If the row overflows (is wider than its constraints), that overflowing width is returned.
  if (total_width < bbox_height(constraints)) total_width = bbox_height(constraints);

  return (point_t){
    .y = total_width,
    .x = total_height,
  };
}


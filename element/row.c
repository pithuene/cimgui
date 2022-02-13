#include "element.h"
#include <stdio.h>

point_t elem_row_draw(AppContext *app, bbox_t constraints, elem_row_t *conf) {
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
    if (conf->items[i].x_align > current_alignment) current_alignment = conf->items[i].x_align;

    child_target_sizes[i] = (point_t){
      .x = unit_length_in_px(conf->items[i].width, bbox_width(constraints)),
      .y = unit_length_in_px(conf->items[i].height, bbox_height(constraints)),
    };
    child_sizes[i] = widget_getsize(app, (bbox_t){.max = child_target_sizes[i]}, conf->items[i].widget);

    total_widget_width += child_sizes[i].x;
    if (i < conf->item_count - 1) total_widget_width += conf->spacing;
    if (current_alignment == align_center)
      center_widget_width += child_sizes[i].x + conf->spacing;
  }
  if (center_widget_width < 0) center_widget_width = 0;
  double total_width = total_widget_width;
  // Is the row wider than its constraints.
  // If so, there are no gaps.
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
        double center_start = (total_width / 2) - (center_widget_width / 2);
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
      .y = constraints.min.y,
    };
    point_t bottomright = point_add(topleft, child_target_sizes[i]);
    bbox_t child_constraints = {.min = topleft, .max = bottomright};

    point_t childsize = widget_draw(app, child_constraints, conf->items[i].widget);

    if (i < conf->item_count - 1) {
      current_width          += childsize.x;
      remaining_widget_width -= childsize.x;

      current_width          += conf->spacing;
      remaining_widget_width -= conf->spacing;
    }
    if (childsize.y > max_height) max_height = childsize.y;
  }

  return (point_t){
    .x = current_width,
    .y = max_height,
  };
}

point_t elem_row_size(AppContext *app, bbox_t constraints, elem_row_t *conf) {
  float total_width  = 0;
  float total_height = 0;

  for (int i = 0; i < conf->item_count; i++) {
    point_t topleft = {
      .x = constraints.min.x + total_width,
      .y = constraints.min.y,
    };
    point_t target_child_size = {
      .x = unit_length_in_px(conf->items[i].width, bbox_width(constraints)),
      .y = unit_length_in_px(conf->items[i].height, bbox_height(constraints)),
    };
    point_t bottomright = point_add(topleft, target_child_size);
    bbox_t child_constraints = {.min = topleft, .max = bottomright};

    point_t childsize = widget_getsize(app, child_constraints, conf->items[i].widget);
    total_width += childsize.x;
    if (i < conf->item_count - 1) total_width += conf->spacing;
    if (childsize.y > total_height) total_height = childsize.y;
  }

  // If the row is not entirely full (gaps), the width is still the full constraint width.
  // If the row overflows (is wider than its constraints), that overflowing width is returned.
  if (total_width < bbox_width(constraints)) total_width = bbox_width(constraints);

  return (point_t){
    .x = total_width,
    .y = total_height,
  };
}


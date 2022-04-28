#include "element.h"
#include <stdbool.h>
#include <stdio.h>

point_t scrollable_column(AppContext *app, point_t constraints, scrollable_column_t *conf) {
  point_t per_item_constraints = (point_t){constraints.x, 0};

  while (conf->scroll_offset < conf->spacing) {
    // The item before first_shown_index is visible
    if (conf->first_shown_index <= 0) {
      // There are no earlier items not shown
      break;
    }

    deferred_draw_t def_draw = widget_draw_deferred(app, per_item_constraints, conf->children[conf->first_shown_index - 1]);
    conf->first_shown_index--;
    conf->scroll_offset = def_draw.dimensions.y + conf->scroll_offset;
  }

  point_t offset = {0, -conf->scroll_offset};
  conf->last_shown_index = conf->child_count - 1; // Expect all child elements to be drawn. Otherwise last_shown_index is set once the constriants are reached.
  for (int i = conf->first_shown_index; i < conf->child_count; i++) {
    point_t size;
    with_offset(&app->oplist, offset){
      size = widget_draw(app, per_item_constraints, conf->children[i]);
    };

    if (i == conf->first_shown_index && conf->scroll_offset > size.y) {
      // First child no longer visible
      if (conf->child_count > conf->first_shown_index + 1) {
        conf->first_shown_index++;
        conf->scroll_offset = conf->scroll_offset - size.y;
      }
    }
    offset.y += size.y + conf->spacing;

    if (offset.y >= constraints.y) {
      // Only render as many elements, as are shown
      conf->last_shown_index = i;
      break;
    }
  }

  return constraints;
}

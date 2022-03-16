#include "widgets.h"

point_t widget_draw(AppContext *app, point_t constraints, widget_t widget) {
  return widget.draw(app, constraints, widget.data);
}

deferred_draw_t widget_draw_deferred(AppContext *app, point_t constraints, widget_t widget) {
  // Backup the original oplist
  oplist_t original_ops = app->oplist;

  // Draw widget into temporary oplist
  app->oplist = oplist_create(&app->oplist);
  point_t dimensions = widget.draw(app, constraints, widget.data);
  deferred_draw_t res = {
    .dimensions = dimensions,
    .ops        = app->oplist,
  };

  // Restore original oplist
  app->oplist = original_ops;

  return res;
}

point_t deferred_draw_execute(AppContext *app, deferred_draw_t draw) {
  oplist_concat(&app->oplist, &draw.ops);
  return draw.dimensions;
}

#include "widgets.h"

point_t widget_draw(AppContext *context, bbox_t contstraints, widget_t *widget) {
  return widget->draw(context, contstraints, widget);
}
point_t widget_getsize(AppContext *context, bbox_t constraints, widget_t *widget) {
  return widget->size(context, constraints, widget);
}

#include "widgets.h"

point_t widget_draw(AppContext *context, widget_t *widget) {
  return widget->draw(context, widget);
}
point_t widget_getsize(AppContext *context, widget_t *widget) {
  return widget->size(context, widget);
}

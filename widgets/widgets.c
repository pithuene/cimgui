#include "widgets.h"

DPoint widget_draw(AppContext *context, widget_t *widget) {
  return widget->draw(context, widget);
}
DPoint widget_getsize(AppContext *context, widget_t *widget) {
  return widget->size(context, widget);
}

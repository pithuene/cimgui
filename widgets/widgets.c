#include "widgets.h"

DPoint draw_widget(AppContext *context, Widget widget) {
  return widget.draw(context, widget.data);
}

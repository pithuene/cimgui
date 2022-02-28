#include "application.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include "events/events.h"
#include "widgets/widgets.h"
#include "element/element.h"
#include "font/font.h"
#include "checktag/checktag.h"
#include "main.h"

editor_t *editor_state;

int main(void) {
  AppContext *app = application_create();

  State state =  {
    .text = "Start: ",
    .textlen = 7,
    .fontSize = 90,
  };

  //state.fontNormal = load_font(app->vg, "Roboto", 0.73, 0.065);
  state.fontNormal = app->font_fallback;

  application_loop(app, (AppLoopFunction) NULL, &state);
  application_free(app);
}

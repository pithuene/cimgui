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

int main(void) {
  AppContext *app = application_create();

  State state =  {
    .text = "HItart: ",
    .textlen = 7,
    .fontSize = 90,
  };

  state.fontNormal = load_font(app->vg, "Roboto", 0.685, 0.115);
  //state.fontNormal = register_font(app, "sans", "/home/pit/code/nanovg-test/nanovg/example/Roboto-Regular.ttf", 0.71, 0.11);

  application_loop(app, (AppLoopFunction) NULL, &state);
  application_free(app);
}

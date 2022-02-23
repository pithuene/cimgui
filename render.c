#include "application.h"
#include "checktag/checktag.h"
#include "element/element.h"
#include "main.h"

void draw(AppContext *app, State *state) {
  check_tag(&state->fontSize);

  point_t window_dimensions = (point_t){app->window.width, app->window.height};
  deferred_draw_t text_draw = widget_draw_deferred(
    app,
    window_dimensions,
    &(widget_t){
      .draw = (widget_draw_t) text,
      .data = &(text_t){
        .color = (color_t){0, 0, 0, 255},
        .content = "Testing something here",
        .font = &state->fontNormal,
        .size = 30,
      }
    }
  );

  with_offset(&app->oplist, (point_t){10, 10}) {
    rect(app, text_draw.dimensions, &(rect_t){.color = {10,200,200,255}});
    deferred_draw_execute(app, text_draw);
  }

  op_offset(&app->oplist, (point_t){100, 100});

  button_t button_template = {
    .background = {200,200,200,255},
    .background_down = {180,180,180,255},
    .background_hover = {210, 210, 210, 255},
    .label_color = {0,0,0,255},
    .label_font = &state->fontNormal,
    .label_font_size = 10,
  };

  bool res1 = false;
  state->btns[0] = button_template;
  state->btns[0].label = "Button 1";
  state->btns[0].result = &res1;
  button(app, window_dimensions, &state->btns[0]);

  op_offset(&app->oplist, (point_t){100, 0});

  bool res2 = false;
  state->btns[1] = button_template;
  state->btns[1].label = "Button 2";
  state->btns[1].result = &res2;
  button(app, window_dimensions, &state->btns[1]);

  op_offset(&app->oplist, (point_t){100, 0});

  bool res3 = false;
  state->btns[2] = button_template;
  state->btns[2].label = "Button 3";
  state->btns[2].result = &res3;
  button(app, window_dimensions, &state->btns[2]);

  op_offset(&app->oplist, (point_t){-200, 100});

  static int sliderval = 50;
  slider(app, window_dimensions, &(slider_t){
    .font = &state->fontNormal,
    .min = 0,
    .max = 100,
    .step = 1,
    .value = &sliderval,
  });

  op_offset(&app->oplist, (point_t){0, 100});

  static int sliderval2 = 50;
  slider(app, window_dimensions, &(slider_t){
    .font = &state->fontNormal,
    .min = 0,
    .max = 100,
    .step = 1,
    .value = &sliderval2,
  });
}

hr_guest_t hr_guest_draw = {
  .draw = (AppLoopFunction) draw,
};

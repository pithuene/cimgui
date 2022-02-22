#include "application.h"
#include "checktag/checktag.h"
#include "element/element.h"
#include "main.h"

void draw(AppContext *app, State *state) {
  check_tag(&state->fontSize);

  bbox_t window_bounds = bbox_from_dims((point_t){0,0}, app->window.width, app->window.height);
  deferred_draw_t d = widget_draw_deferred(
    app,
    window_bounds,
    &(widget_t){
      .draw = (widget_draw_t) text,
      .data = &(text_t){
        .color = (color_t){0, 0, 0, 255},
        .content = "Testing",
        .font = &state->fontNormal,
        .size = 30,
      }
    }
  );
  rect(app, (bbox_t){{0}, d.dimensions}, &(rect_t){.color = {10,200,200,255}});
  deferred_draw_execute(app, d);
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
  state->btns[0].label = "Button";
  state->btns[0].result = &res1;
  button(app, window_bounds, &state->btns[0]);

  op_offset(&app->oplist, (point_t){100, 0});

  bool res2 = false;
  state->btns[1] = button_template;
  state->btns[1].label = "Button 2";
  state->btns[1].result = &res2;
  button(app, window_bounds, &state->btns[1]);

  op_offset(&app->oplist, (point_t){100, 0});

  bool res3 = false;
  state->btns[2] = button_template;
  state->btns[2].label = "Button 3";
  state->btns[2].result = &res3;
  button(app, window_bounds, &state->btns[2]);

  /*widget_draw(app, window_bounds, 
    &(widget_t){
      .draw = (widget_draw_t) text,
      .data = &(text_t){
        .color = (color_t){0, 0, 0, 255},
        .content = "Test",
        .font = &state->fontNormal,
        .size = 30,
      }
    }
  );*/
}

hr_guest_t hr_guest_draw = {
  .draw = (AppLoopFunction) draw,
};

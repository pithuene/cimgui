#include "application.h"
#include "checktag/checktag.h"
#include "element/element.h"
#include "main.h"
#include "piecetable/editor.h"
#include <stdio.h>

point_t sidebar(AppContext *app, point_t constraints, void *conf) {
  rect(app, constraints, &(rect_t){(color_t){237,242,247,255}});
  with_offset(&app->oplist, (point_t){20,20}) {
    column(app, constraints, &(column_t){
      .spacing = 0,
      .children = element_children(
        {
          .widget = widget(text, &(text_t){
            .color = (color_t){0, 0, 0, 255},
            .content = "Title",
            .font = &app->font_fallback,
            .size = 20,
          }),
        },
      ),
    });
  }
  return constraints;
}



point_t editingarea(AppContext *app, point_t constraints, void *conf) {
  rect(app, constraints, &(rect_t){(color_t){255,255,255,255}});

  char *content = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.";

  static int spacing = 6;
  with_offset(&app->oplist, (point_t){300,50}) {
    slider(app, (point_t){0}, &(slider_t){
      .font = &app->font_fallback,
      .min = 0,
      .max = 100,
      .step = 1,
      .value = &spacing,
    });
  }

  static int font_size = 12;
  with_offset(&app->oplist, (point_t){600,50}) {
    slider(app, (point_t){0}, &(slider_t){
      .font = &app->font_fallback,
      .min = 0,
      .max = 100,
      .step = 1,
      .value = &font_size,
    });
  }

  if (!editor_state) {
    editor_state = malloc(sizeof(editor_t));
    *editor_state = editor_create("Some initial content and here is some more");
  }


  with_offset(&app->oplist, (point_t){300,100}) {
    column(app, (point_t){.x = 500}, &(column_t){
      .children = element_children(
        {
          .width = {100, unit_percent},
          .widget = widget(paragraph, &(paragraph_t){
            .color = (color_t){0,0,255,255},
            .content = content,
            .font = &app->font_fallback,
            .max_rows = 10,
            .size = font_size,
            .spacing = spacing,
          }),
        },
        {
          .width = {100, unit_percent},
          .height = {100, unit_px},
          .x_align = align_center,
          .widget = widget(rect, &(rect_t){
            .color = (color_t){0,0,255,128},
          })
        },
        {
          .width = {100, unit_percent},
          .height = {300, unit_px},
          .x_align = align_center,
          .widget = widget(editor, editor_state),
        },
      ),
    });
  }
  return constraints;
}

point_t mainlayout(AppContext *app, point_t constraints, void *conf) {
  row(app, constraints, &(row_t){
    .spacing = 0,
    .children = element_children(
      {
        .width = {15, unit_percent},
        .height = {100, unit_percent},
        .widget = widget(sidebar, NULL),
      },
      {
        .width = {85, unit_percent},
        .height = {100, unit_percent},
        .widget = widget(editingarea, NULL),
      },
    ),
  });
  return constraints;
}

void draw(AppContext *app, State *state) {
  point_t window_dimensions = (point_t){app->window.width, app->window.height};
  mainlayout(app, window_dimensions, NULL);
}

hr_guest_t hr_guest_draw = {
  .draw = (AppLoopFunction) draw,
};

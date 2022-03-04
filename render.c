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

  if (!editor_state) {
    editor_state = malloc(sizeof(editor_t));
    *editor_state = editor_create("Some initial content and here is some more");
  }

  with_offset(&app->oplist, (point_t){unit_length_in_px((unit_length_t){25, unit_percent}, constraints.x),100}) {
    editor(
      app,
      (point_t){
        .x = unit_length_in_px((unit_length_t){50, unit_percent}, constraints.x),
        .y = constraints.y,
      },
      editor_state
    );
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

#include <stdio.h>
#include "editor.h"
#include "../widgets/widgets.h"
#include "../element/element.h"

typedef struct {
  editor_t *ed;
  block_t  *block;
} block_draw_data_t;

point_t draw_paragraph(AppContext *app, point_t constraints, block_draw_data_t* data) {
  block_paragraph_t *pgraph = (block_paragraph_t*) data->block;
  char buffer[1024] = {0}; // TODO: If there is over 1k of text in a block, this will fail
  char *encoding_ptr = (char*) buffer;
  piecetable_piece_t *curr_piece = pgraph->block.first_piece;
  while (curr_piece) {
    rune_t *source = (curr_piece->from_original)
      ? data->ed->original
      : data->ed->added;
    for (int i = 0; i < curr_piece->length; i++) {
      // TODO: Hack until proper cursor rendering is implemented
      if (curr_piece == data->ed->cursor.piece
        && i == data->ed->cursor.offset) {
         *encoding_ptr = '|';
         encoding_ptr++;
         // Doesnt work!!
      }
      const int index = curr_piece->start + i;
      rune_encode(&encoding_ptr, source[index]);

    }

    curr_piece = curr_piece->next;
  }

  return paragraph(app, constraints, &(paragraph_t){
    .color = (color_t){0,0,0,255},
    .content = buffer,
    .font = &app->font_fallback,
    .max_rows = 10, // TODO: Das wird noch problematisch
    .size = 12,
    .spacing = 6,
  });
}

widget_draw_t draw_function_for_type(blocktype_t type) {
  switch (type) {
    case blocktype_paragraph: return (widget_draw_t) draw_paragraph;
    default: {
      fprintf(stderr, "Editor draw function for block type %d not defined!\n", type);
      exit(1);
    }
  }
}

point_t editor(AppContext *app, point_t constraints, editor_t *ed) {
  eventqueue_foreach(InputEvent event, app->eventqueue) {
    if (event.type == eventtype_key) {
      KeyEvent keyevent = event.instance.key;
      if (keyevent.action == ButtonActionPress || keyevent.action == ButtonActionRepeat) {
        if (keyevent.key == GLFW_KEY_LEFT) {
          editor_move_cursor_backward(ed, &ed->cursor);
        } else if (keyevent.key == GLFW_KEY_RIGHT) {
          editor_move_cursor_forward(ed, &ed->cursor);
        }
      }
    }
  }

  int block_count = editor_block_count(ed);

  element_t block_elements[block_count];

  block_t *curr_block = ed->first;
  int block_index = 0;
  while (curr_block) {
    widget_t *block_widget = arenaalloc(&app->ops_arena, sizeof(widget_t));
    block_draw_data_t *draw_data = arenaalloc(&app->ops_arena, sizeof(block_draw_data_t));
    *draw_data = (block_draw_data_t){
      .ed = ed,
      .block = curr_block,
    };
    *block_widget = (widget_t){
      .draw = draw_function_for_type(curr_block->type),
      .data = draw_data
    };
    block_elements[block_index] = (element_t){
      .width = {100, unit_percent},
      .widget = block_widget,
    };
    curr_block = curr_block->next;
  }

  column(app, constraints, &(column_t){
    .spacing = 20,
    .children = (element_children_t){
      .count = block_count,
      .elements = block_elements,
    },
  });
  return constraints;
}

#include <stdio.h>
#include <assert.h>
#include "editor.h"
#include "../widgets/widgets.h"
#include "../element/element.h"
#include "../ops/ops.h"

typedef struct {
  editor_t *ed;
  block_t  *block;
} block_draw_data_t;

typedef struct {
  Font       *font;
  float       font_size;
  const char *content;
  const char *content_end;
  color_t     color;
  // The index of the caret in this row. Negativ if there is none.
  int16_t     caret;
} editable_row_t;

point_t editable_row(AppContext *app, point_t constraints, editable_row_t *conf) {
  op_begin_path(&app->oplist);
  op_fill_color(&app->oplist, conf->color);

  Font *font = conf->font;
  if (!font) {
    font = &app->font_fallback;
  }

  point_t bounds = text_bounds(app->vg, conf->font_size, font, conf->content, conf->content_end);

  op_text(&app->oplist, conf->font_size, font, conf->content, conf->content_end);

  if (conf->caret >= 0) {
    // Caret is in this row
    glyph_position_t glyph_positions[conf->caret + 1];
    text_glyph_positions(
      app->vg,
      conf->font_size,
      font,
      conf->content,
      conf->content + conf->caret + 1,
      glyph_positions,
      conf->caret + 1
    );

    float caret_x_offset = glyph_positions[conf->caret].minx;
    with_offset(&app->oplist, (point_t){caret_x_offset, -conf->font_size * 0.25}) {
      rect(
        app,
        (point_t){
          .x = 1,
          .y = conf->font_size*1.5
        },
        &(rect_t){
          .color = (color_t){0,0,0,255}
        }
      );
    }
  }

  return bounds;
}

typedef struct {
  Font *font;
  float font_size;
  editor_t *ed;
  piecetable_piece_t *first_piece;
  color_t color;
  editor_cursor_t cursor;
} editable_text_t;

point_t editable_text(AppContext *app, point_t constraints, editable_text_t *conf) {
  // TODO: If there is over 1k of text in a block, this will fail
  #define BUFFER_LEN 1024
  char buffer[BUFFER_LEN] = {0};
  char *encoding_ptr = (char*) buffer;
  char *caret = NULL;
  piecetable_piece_t *curr_piece = conf->first_piece;
  while (curr_piece) {
    rune_t *source = (curr_piece->from_original)
      ? conf->ed->original
      : conf->ed->added;
    for (int i = 0; i < curr_piece->length; i++) {
      if (curr_piece == conf->cursor.piece
        && i == conf->cursor.offset) {
        caret = encoding_ptr;
      }
      assert(encoding_ptr < buffer + BUFFER_LEN);
      const int index = curr_piece->start + i;
      rune_encode(&encoding_ptr, source[index]);
    }

    curr_piece = curr_piece->next;
  }

  const int max_rows = 20;
  text_line_t lines[max_rows];
  int row_count = text_break_lines(
    app->vg,
    conf->font,
    conf->font_size,
    buffer,
    encoding_ptr,
    constraints.x,
    lines,
    max_rows
  );

  element_t text_elements[row_count];

  for (int j = 0; j < row_count; j++) {
    editable_row_t *text_element = arenaalloc(&app->ops_arena, sizeof(editable_row_t));
    size_t content_length = lines[j].end - lines[j].start;
    char *content = arenaalloc(&app->ops_arena, content_length);
    strncpy(content, lines[j].start, content_length);

    int16_t caret_idx = -1;
    if (lines[j].start <= caret && caret <= lines[j].end) {
      caret_idx = caret - lines[j].start;
    }

    *text_element = (editable_row_t){
      .color = (color_t){0,0,0,255},
      .content = content,
      .content_end = content + content_length,
      .font = &app->font_fallback,
      .font_size = conf->font_size,
      .caret = caret_idx,
    };

    widget_t *widget = arenaalloc(&app->ops_arena, sizeof(widget_t));
    *widget = (widget_t){
        (widget_draw_t) editable_row,
        text_element,
    };

    text_elements[j] = (element_t){.widget = widget};
  }

  return column(app, constraints, &(row_t){
    .spacing  = 6,
    .children = (element_children_t){
      .count    = row_count,
      .elements = text_elements,
    }
  });
}

point_t draw_paragraph(AppContext *app, point_t constraints, block_draw_data_t* data) {
  return editable_text(app, constraints, &(editable_text_t){
    .color = (color_t){0,0,0,255},
    .ed = data->ed,
    .first_piece = data->block->first_piece,
    .font = &app->font_fallback,
    .font_size = 12,
    .cursor = data->ed->cursor,
  });
}

point_t draw_heading(AppContext *app, point_t constraints, block_draw_data_t* data) {
  block_heading_t *heading = (block_heading_t *) data->block;

  float font_sizes[7] = {
    48,
    24,
    20,
    18,
    16,
    14,
    12,
  };

  int level = heading->level - 1;
  clamp_int(&level, 0, 6);

  return editable_text(app, constraints, &(editable_text_t){
    .color = (color_t){0,0,0,255},
    .ed = data->ed,
    .first_piece = data->block->first_piece,
    .font = &app->font_fallback,
    .font_size = font_sizes[level],
    .cursor = data->ed->cursor,
  });
}

widget_draw_t draw_function_for_type(blocktype_t type) {
  switch (type) {
    case blocktype_paragraph: return (widget_draw_t) draw_paragraph;
    case blocktype_heading: return (widget_draw_t) draw_heading;
    default: {
      fprintf(stderr, "Editor draw function for block type %d not defined!\n", type);
      exit(1);
    }
  }
}

point_t editor(AppContext *app, point_t constraints, editor_t *ed) {
  eventqueue_foreach(InputEvent event, app->eventqueue) {
    if (event.type == eventtype_char) {
      CharEvent charevent = event.instance.character;
      editor_insert_before(ed, &ed->cursor, charevent.rune);
    }
    if (event.type == eventtype_key) {
      KeyEvent keyevent = event.instance.key;
      if (keyevent.action == ButtonActionPress || keyevent.action == ButtonActionRepeat) {
        if (keyevent.key == GLFW_KEY_LEFT) {
          editor_move_cursor_backward(ed, &ed->cursor);
        } else if (keyevent.key == GLFW_KEY_RIGHT) {
          editor_move_cursor_forward(ed, &ed->cursor);
        } else if (keyevent.key == GLFW_KEY_BACKSPACE) {
          editor_delete_backwards(ed, &ed->cursor);
        } else if (keyevent.key == GLFW_KEY_ENTER) {
          if (keyevent.mods & GLFW_MOD_SHIFT) {
            rune_t newline = '\n';
            newline <<= 24;
            editor_insert_before(ed, &ed->cursor, newline);
          } else {
            editor_split_block_at_cursor(ed, &ed->cursor);
          }
        } else if (keyevent.key == GLFW_KEY_1 && keyevent.mods & GLFW_MOD_CONTROL) {
          block_t * heading = (block_t *) editor_create_block_heading(ed, 1, NULL, NULL);
          editor_block_turn_into(ed, ed->cursor.block, heading);
          ed->cursor.block = heading;
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
    block_index++;
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
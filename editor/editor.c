#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include "editor.h"
#include "../widgets/widgets.h"
#include "../element/element.h"
#include "../ops/ops.h"
#include "../filedialog/filedialog.h"
#include "../utils/logging.h"
#include "../font/utf8/utf8.h"

// Open a file selection dialog and import the selected markdown file.
static void select_and_open_file(editor_t *ed) {
  nfdchar_t *path = NULL;
  nfdresult_t dialog_result = NFD_OpenDialog(NULL, NULL, &path);
  if (dialog_result == NFD_OKAY) {
    editor_open_file(ed, path);
    free(path);
  }
}

typedef struct {
  editor_t *ed;
  block_t  *block;
} block_draw_data_t;

typedef struct {
  int length;
  rune_position_t *positions;
} piece_rune_positions_t;

typedef struct {
  editor_t *ed;
  editor_cursor_t cursor;
  piecetable_piece_t *piece;
  uint32_t start;
  uint32_t length;
  color_t color;
  float font_size;
  Font *font;
} editable_piece_part_t;

void draw_caret(AppContext *app, float font_size) {
  rect(
    app,
    (point_t){
      .x = 1,
      .y = font_size*1.5
    },
    &(rect_t){
      .color = (color_t){0,0,0,255}
    }
  );
}

static inline void calculate_piece_rune_positions(
  AppContext *app,
  float font_size,
  Font *font,
  editor_t *ed,
  piecetable_piece_t *piece,
  rune_position_t *positions_buf,
  piece_rune_positions_t *positions
) {
  const rune_t *source = piece->from_original ? ed->original : ed->added;
  int length = rune_text_positions(
    app->vg,
    font_size,
    font,
    source + piece->start,
    source + piece->start + piece->length,
    positions_buf,
    piece->length
  );
  *positions = (piece_rune_positions_t){
    .positions = positions_buf,
    .length = length,
  };
}

// Create a text widget from part of a piece or a complete piece.
point_t editable_piece_part(AppContext *app, point_t constraints, editable_piece_part_t *conf) {
  debug_log("Rendering part of piece %p from source %s, starting at %d and going until %d \n",
    (void *) conf->piece,
    conf->piece->from_original ? "original" : "added",
    conf->piece->start + conf->start,
    conf->piece->start + conf->start + conf->length
  );
  rune_t *source = conf->piece->from_original ? conf->ed->original : conf->ed->added;

  point_t text_dimensions = rune_text(app, constraints, &(rune_text_t){
    .content     = source + conf->piece->start + conf->start,
    .content_end = source + conf->piece->start + conf->start + conf->length,
    .color = conf->color,
    .size = conf->font_size,
    .font = conf->font,
  });

  if (conf->cursor.piece == conf->piece
    && conf->cursor.offset >= conf->start
    && conf->cursor.offset < conf->start + conf->length)
  {
    // Cursor is in this piece part

    rune_position_t positions_buf[conf->piece->length];
    piece_rune_positions_t positions = {
      .length = conf->piece->length,
      .positions = positions_buf,
    };
    calculate_piece_rune_positions(
      app,
      conf->font_size,
      conf->font,
      conf->ed,
      conf->piece,
      positions_buf,
      &positions
    );
    assert(positions.length == conf->piece->length);

    assert(conf->length <= positions.length);
    assert(conf->cursor.offset < positions.length);
    float cursor_x_offset = positions.positions[conf->cursor.offset].minx - positions.positions[conf->start].minx;
    with_offset(&app->oplist, (point_t){cursor_x_offset, -conf->font_size * 0.25}) {
      draw_caret(app, conf->font_size);
    }
  }

  return text_dimensions;
}

typedef struct {
  Font *font;
  float font_size;
  editor_t *ed;
  piecetable_piece_t *first_piece;
  color_t color;
  editor_cursor_t cursor;
  float line_spacing;
} editable_text_t;

// TODO: Might be simpler to just reuse the editor_cursor_t for this
typedef struct {
  piecetable_piece_t *piece;
  uint32_t            offset; // TODO: Rename this so the difference to a piecetable_piece_t offset is clear
} pieceposition_t;

typedef struct {
  pieceposition_t start;
  pieceposition_t end;
} editable_line_t;

static pieceposition_t pieceposition_next(pieceposition_t curr) {
  if (curr.piece->length > curr.offset + 1) {
    return (pieceposition_t){
      .piece = curr.piece,
      .offset = curr.offset + 1,
    };
  }
  if (curr.piece->next) {
    return (pieceposition_t){
      .piece = curr.piece->next,
      .offset = 0,
    };
  }
  // This is the last rune
  return curr;
}

static pieceposition_t pieceposition_last(pieceposition_t curr) {
  if (curr.offset > 1) {
    return (pieceposition_t){
      .piece = curr.piece,
      .offset = curr.offset - 1,
    };
  }
  if (curr.piece->prev) {
    return (pieceposition_t){
      .piece = curr.piece->prev,
      .offset = curr.piece->prev->length - 1,
    };
  }
  // This is the first rune
  return curr;
}

static rune_t pieceposition_rune(editor_t *ed, pieceposition_t pos) {
  const rune_t *source = pos.piece->from_original ? ed->original : ed->added;
  return source[pos.piece->start + pos.offset];
}

static inline bool piecepositions_equal(pieceposition_t a, pieceposition_t b) {
  return a.piece == b.piece && a.offset == b.offset;
}

// Returns the pieceposition_t of the first whitespace after the next word
// Also returns if rune after the next word is a newline
// and if the word is empty (if there are multiple newlines, the words in between have no width), in this case the end is invalid.
static pieceposition_t find_next_word_end(editor_t *ed, pieceposition_t start, bool *new_line, bool *word_empty) {
  pieceposition_t curr = start;
  pieceposition_t next;

  if (rune_is_newline(pieceposition_rune(ed, start))) {
    *new_line = true;
    *word_empty = true;
    return start;
  }

  // Skip initial whitespace
  while (rune_is_whitespace(pieceposition_rune(ed, curr))) {
    next = pieceposition_next(curr);
    if (piecepositions_equal(next, curr)) { // Position didn't move => This is the last rune
      return curr;
    }
    curr = next;
  }

  // Skip over the next word
  while (!rune_is_whitespace(pieceposition_rune(ed, curr)) && !rune_is_newline(pieceposition_rune(ed, curr))) {
    next = pieceposition_next(curr);
    if (piecepositions_equal(next, curr)) { // Position didn't move => This is the last rune
      return curr;
    }
    curr = next;
  }

  if (pieceposition_rune(ed, curr) == '\n' << 24) { // Newline after word
    *new_line = true;
  } else {
    *new_line = false;
  }

  return curr;
}


static float calculate_word_width(
  AppContext *app,
  float font_size,
  Font *font,
  editor_t *ed,
  pieceposition_t start,
  pieceposition_t end
) {
  float width = 0;
  
  // Word in a single piece
  if (start.piece == end.piece) {
    const rune_t *source = start.piece->from_original ? ed->original : ed->added;
    point_t bounds = rune_text_bounds(
      app->vg,
      font_size,
      font,
      source + start.piece->start + start.offset,
      source + start.piece->start + end.offset);
    return bounds.x;
  }

  /* End of start piece */ {
    const rune_t *source = start.piece->from_original ? ed->original : ed->added;
    point_t bounds = rune_text_bounds(
      app->vg,
      font_size,
      font,
      source + start.piece->start + start.offset,
      source + start.piece->start + start.piece->length);
    width += bounds.x;
  }

  // Handle all middle pieces
  piecetable_piece_t *curr_piece = start.piece->next;
  while (curr_piece && curr_piece != end.piece) {
    const rune_t *source = start.piece->from_original ? ed->original : ed->added;
    point_t bounds = rune_text_bounds(
      app->vg,
      font_size,
      font,
      source + curr_piece->start,
      source + curr_piece->start + curr_piece->length);
    width += bounds.x;

    curr_piece = curr_piece->next;
  }

  editor_check_health(ed);
  // End piece is not reachable from start piece
  assert(curr_piece == end.piece);
  assert(curr_piece != NULL);

  /* Handle start of end piece */ {
    const rune_t *source = start.piece->from_original ? ed->original : ed->added;
    point_t bounds = rune_text_bounds(
      app->vg,
      font_size,
      font,
      source + end.piece->start,
      source + end.piece->start + end.offset);
    width += bounds.x;
  }

  return width;
}

static void draw_editable_line(AppContext *app, editor_t *ed, editable_line_t line, editable_piece_part_t part_template) {
  pieceposition_t curr_position = line.start;
  // The width of all previous parts in this line. Used for offset calculation.
  float x_offset = 0;
  while (curr_position.piece && curr_position.piece != line.end.piece) {
    editable_piece_part_t part = part_template;
    part.piece = curr_position.piece;
    part.start = curr_position.offset;
    part.length = curr_position.piece->length - curr_position.offset;

    point_t part_offset = {x_offset, 0};
    with_offset(&app->oplist, part_offset){
      x_offset += editable_piece_part(app, (point_t){0,0}, &part).x;
    };

    curr_position = (pieceposition_t){
      .piece = curr_position.piece->next,
      .offset = 0,
    };
  }

  assert(curr_position.piece != NULL);
  assert(curr_position.piece == line.end.piece);

  // Rendering the last piece of the line
  editable_piece_part_t part = part_template;
  part.piece = curr_position.piece;
  part.start = curr_position.offset;
  part.length = line.end.offset - curr_position.offset + 1;

  point_t part_offset = {x_offset, 0};
  with_offset(&app->oplist, part_offset){
    x_offset += editable_piece_part(app, (point_t){0,0}, &part).x;
  }
}

point_t editable_text(AppContext *app, point_t constraints, editable_text_t *conf) {
  editable_piece_part_t part_template = {
    .font = conf->font,
    .font_size = conf->font_size,
    .color = conf->color,
    .cursor = conf->cursor,
    .ed = conf->ed,
  };

  // Break into lines
  float remaining_row_width = constraints.x;
  pieceposition_t current_line_start = {
    .piece = conf->first_piece,
    .offset = 0,
  };
  pieceposition_t current_line_end = current_line_start;

  pieceposition_t curr_rune = current_line_start; // The next rune to be handled

  // Number of lines above the current one. Used for offset calculation.
  int lines_drawn = 0;

  while (1) { // Loop over words
    if (!curr_rune.piece->next && curr_rune.offset >= curr_rune.piece->length - 1) {
      // End of text reached
      with_offset(&app->oplist, (point_t){0, lines_drawn * (conf->font_size + conf->line_spacing)}) {
        draw_editable_line(app, conf->ed, (editable_line_t){
          .start = current_line_start,
          .end = current_line_end,
        }, part_template);
      }
      lines_drawn++;

      break;
    }

    bool new_line = false;
    bool word_empty = false;
    pieceposition_t next_word_end = find_next_word_end(conf->ed, curr_rune, &new_line, &word_empty);

    // TODO: What if the word is longer than a line

    if (!word_empty) {
      float next_word_width = calculate_word_width(
        app,
        conf->font_size,
        conf->font,
        conf->ed,
        curr_rune,
        next_word_end
      );

      if (remaining_row_width >= next_word_width) { // Next word fits in this line
        current_line_end = next_word_end;
        remaining_row_width -= next_word_width;
        curr_rune = pieceposition_next(current_line_end);
        //curr_rune = pieceposition_next(curr_rune); // Skip the whitespace character after this word
      } else {
        // New row
        with_offset(&app->oplist, (point_t){0, lines_drawn * (conf->font_size + conf->line_spacing)}) {
          draw_editable_line(app, conf->ed, (editable_line_t){
            .start = current_line_start,
            .end = current_line_end,
          }, part_template);
        }

        lines_drawn++;
        current_line_start = curr_rune;
        current_line_end = next_word_end;
        curr_rune = pieceposition_next(next_word_end);
        remaining_row_width = constraints.x - next_word_width;
      }
    }

    if (new_line) {
      if (!word_empty) {
        // New row
        with_offset(&app->oplist, (point_t){0, lines_drawn * (conf->font_size + conf->line_spacing)}) {
          draw_editable_line(app, conf->ed, (editable_line_t){
            .start = current_line_start,
            .end = current_line_end,
          }, part_template);
        }
      }
      lines_drawn++;

      current_line_start = curr_rune;
      current_line_end = curr_rune;
      remaining_row_width = constraints.x;
    }
  }

  return (point_t){
    .x = constraints.x,
    .y = lines_drawn * conf->font_size + (lines_drawn - 1) * conf->line_spacing,
  };
}

point_t draw_paragraph(AppContext *app, point_t constraints, block_draw_data_t* data) {
  return editable_text(app, constraints, &(editable_text_t){
    .color = (color_t){0,0,0,255},
    .ed = data->ed,
    .first_piece = data->block->first_piece,
    .font = &app->font_fallback,
    .font_size = 12,
    .cursor = data->ed->cursor,
    .line_spacing = 12,
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
    .line_spacing = font_sizes[level],
  });
}

point_t draw_bullet(AppContext *app, point_t constraints, block_draw_data_t* data) {
  block_bullet_t *bullet = (block_bullet_t*) data->block;
  unit_length_t left_padding = {bullet->indentation_level * 20, unit_px};
  unit_length_t bullet_point_width = {10, unit_px};
  unit_length_t content_width = {constraints.x - bullet_point_width.size - left_padding.size, unit_px};

  return row(app, constraints, &(row_t){
    .spacing  = 6,
    .children = element_children(
        {
          .width = left_padding,
          .widget = widget(rect, &(circle_t){
            .color = (color_t){0,0,0,0},
          })
        },
        {
          .width = bullet_point_width,
          .height = {5, unit_px},
          .y_align = align_center,
          .widget = widget(circle, &(circle_t){
            .color = (color_t){0,0,0,255},
          })
        },
        {
          .width = content_width,
          .widget = widget(editable_text, &(editable_text_t){
            .color = (color_t){0,0,0,255},
            .ed = data->ed,
            .first_piece = data->block->first_piece,
            .font = &app->font_fallback,
            .font_size = 12,
            .cursor = data->ed->cursor,
            .line_spacing = 12,
          })
        }
    ),
  });
}

widget_draw_t draw_function_for_type(blocktype_t type) {
  switch (type) {
    case blocktype_paragraph: return (widget_draw_t) draw_paragraph;
    case blocktype_heading: return (widget_draw_t) draw_heading;
    case blocktype_bullet: return (widget_draw_t) draw_bullet;
    default: {
      fprintf(stderr, "Editor draw function for block type %d not defined!\n", type);
      exit(1);
    }
  }
}


point_t editor(AppContext *app, point_t constraints, editor_t *ed) {
  static float scroll_offset = 0;

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
        } else if (keyevent.key == GLFW_KEY_DOWN) {
          if (ed->cursor.block->next) {
            // TODO: Actually move cursor to the closest position in the row below
            ed->cursor = (editor_cursor_t){
              .block = ed->cursor.block->next,
              .piece = ed->cursor.block->next->first_piece,
              .offset = 0,
            };
          }
        } else if (keyevent.key == GLFW_KEY_UP) {
          if (ed->cursor.block->prev) {
            // TODO: Actually move cursor to the closest position in the row above
            ed->cursor = (editor_cursor_t){
              .block = ed->cursor.block->prev,
              .piece = ed->cursor.block->prev->first_piece,
              .offset = 0,
            };
          }
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
        } else if (keyevent.key == GLFW_KEY_L && keyevent.mods & GLFW_MOD_CONTROL) {
          block_t * bullet = (block_t *) editor_create_block_bullet(ed, 0, NULL, NULL);
          editor_block_turn_into(ed, ed->cursor.block, bullet);
          ed->cursor.block = bullet;
        } else if (keyevent.key == GLFW_KEY_TAB) {
          if (ed->cursor.block->type == blocktype_bullet) {
            // List indentation
            block_bullet_t *bullet_block = (block_bullet_t *) ed->cursor.block;
            if (keyevent.mods & GLFW_MOD_SHIFT) {
              // Move the current list entry aswell as all child entries back
              vec_t(block_bullet_t *) entries_to_move = vec(block_bullet_t *, 16);
              vecpush(entries_to_move, (block_bullet_t *) ed->cursor.block);
              if (bullet_block->indentation_level > 0 // Only move children if you are actually moving the block under the cursor
                  && bullet_block->block.next
                  && bullet_block->block.next->type == blocktype_bullet
                  && ((block_bullet_t *) bullet_block->block.next)->indentation_level > bullet_block->indentation_level)
              {
                // There are child elements, add them to the list of entries to move
                for (block_bullet_t *curr_child = (block_bullet_t *) bullet_block->block.next;
                     curr_child && curr_child->block.type == blocktype_bullet && curr_child->indentation_level > bullet_block->indentation_level;
                     curr_child = (block_bullet_t *) curr_child->block.next)
                {
                  vecpush(entries_to_move, curr_child);
                }
              }

              for (int i = 0; i < veclen(entries_to_move); i++) {
                if (entries_to_move[i]->indentation_level > 0) entries_to_move[i]->indentation_level--;
              }
            } else {
              block_bullet_t *previous_bullet = NULL;
              if (bullet_block->block.prev->type == blocktype_bullet) previous_bullet = (block_bullet_t *) bullet_block->block.prev;

              if (previous_bullet && previous_bullet->indentation_level >= bullet_block->indentation_level) {
                if (bullet_block->indentation_level < 10) bullet_block->indentation_level++;
              }
            }
          }
        } else if (keyevent.key == GLFW_KEY_O && keyevent.mods & GLFW_MOD_CONTROL) {
          pthread_t file_open_thread;
          pthread_create(&file_open_thread, NULL, (void *(*)(void *)) select_and_open_file, (void *) ed);
        } else if (keyevent.key == GLFW_KEY_S && keyevent.mods & GLFW_MOD_CONTROL) {
          if (!editor_save_file(ed)) {
            printf("Could not save\n");
          }
        } else if (keyevent.key == GLFW_KEY_H && keyevent.mods & GLFW_MOD_CONTROL) {
          printf("Checking editor health:\n");
          editor_check_health(ed);
        } else if (keyevent.key == GLFW_KEY_S && keyevent.mods & GLFW_MOD_CONTROL) {
          editor_export_markdown(ed, stdout);
        } else if (keyevent.key == GLFW_KEY_PAGE_UP) {
          scroll_offset += 10;
        } else if (keyevent.key == GLFW_KEY_PAGE_DOWN) {
          scroll_offset -= 10;
        }
      }
    }
  }

  int block_count = editor_block_count(ed);

  block_draw_data_t block_draw_datas[block_count];
  widget_t block_widgets[block_count];

  // The index of the block the cursor is currently in
  int cursor_block_index = 0;

  block_t *curr_block = ed->first;
  int block_index = 0;
  while (curr_block) {
    // Find cursor_block_index
    if (curr_block == ed->cursor.block) {
      cursor_block_index = block_index;
    }

    block_draw_datas[block_index] = (block_draw_data_t){
      .ed = ed,
      .block = curr_block,
    };
    block_widgets[block_index] = (widget_t){
      .draw = draw_function_for_type(curr_block->type),
      .data = &block_draw_datas[block_index],
    };
    curr_block = curr_block->next;
    block_index++;
  }

  static scrollable_column_t column = {
    .child_count = 0,
    .children = NULL,
    .spacing = 20,
    .first_shown_index = 5,
    .last_shown_index = 99999999, // Set to large value so the first draw doesn't think the cursor isn't visible
    .scroll_offset = 0,
  };

  column.children = block_widgets;
  column.child_count = block_count;

  // TODO: What if a block is taller than the view?
  // Is cursor in view?
  if (cursor_block_index < column.first_shown_index) {
    // Cursor above current view
    column.first_shown_index = cursor_block_index;
    column.scroll_offset = 0;
  }

  scrollable_column(app, constraints, &column);

  if (cursor_block_index > column.last_shown_index) {
    // Cursor is at the end of or below the current view

    // TODO: Since I don't know the height of the block, this doesn't make sure that the block is visible entirely, only that part of it is.
    if (column.first_shown_index < column.child_count - 1) {
      column.first_shown_index++;
    }
  }

  return constraints;
}


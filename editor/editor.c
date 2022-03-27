#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include "editor.h"
#include "../widgets/widgets.h"
#include "../element/element.h"
#include "../ops/ops.h"
#include "../filedialog/filedialog.h"
#include "../utils/logging.h"

// Open a file selection dialog and import the selected markdown file.
static void editor_open_file(editor_t *ed) {
  nfdchar_t *path = NULL;
  nfdresult_t dialog_result = NFD_OpenDialog(NULL, NULL, &path);
  if (dialog_result == NFD_OKAY) {
    editor_clear(ed);

    int fd = open(path, O_RDONLY);
    struct stat stats;
    fstat(fd, &stats);
    const char *file_content = mmap(NULL, stats.st_size, PROT_READ, MAP_SHARED, fd, 0);
    editor_import_markdown(ed, file_content);
    munmap((void *) file_content, stats.st_size);
    free(path);

    editor_delete_block(ed, ed->first);
    ed->cursor = (editor_cursor_t){
      .block = ed->first,
      .piece = ed->first->first_piece,
      .offset = 0,
    };
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
  piece_rune_positions_t piece_rune_positions;
  uint32_t start;
  uint32_t length;
  color_t color;
  float font_size;
  Font *font;
} editable_piece_part_t;

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
    point_t dimensions_upto_cursor = rune_text_bounds(
      app->vg,
      conf->font_size,
      conf->font,
      &source[conf->piece->start + conf->cursor.piece->start],
      &source[conf->piece->start + conf->cursor.piece->start + conf->cursor.offset]
    );
    with_offset(&app->oplist, (point_t){dimensions_upto_cursor.x, -conf->font_size * 0.25}) {
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

  return text_dimensions;
}

typedef struct {
  Font *font;
  float font_size;
  editor_t *ed;
  piecetable_piece_t *first_piece;
  color_t color;
  editor_cursor_t cursor;
} editable_text_t;

typedef struct {
  piecetable_piece_t *piece;
  uint32_t            offset; // TODO: Rename this so the difference to a piecetable_piece_t offset is clear
} pieceposition_t;

typedef struct {
  pieceposition_t start;
  pieceposition_t end;
} editable_line_t;

// Starting from a given position in a piece, calculates the longest part of it that will still fit into a given width.
// Returns the pieceposition of the last rune that will fit.
// If the remainder of the piece fits entirely, it will return the position of the last rune in that piece.
static void find_longest_fitting_piece_part(
  pieceposition_t start, // First rune to be rendered
  piece_rune_positions_t positions, // Rune positions of the piece
  float space_left, // Maximum width to occupy
  pieceposition_t *end, // Result into which the last rendered rune is written
  bool *row_full, // Whether the piece had to be broken up because the row was full
  float *space_occupied // How much width the part occupied
) {
  *end = start;
  float curr_width = positions.positions[end->offset].maxx - positions.positions[end->offset].minx;
  while (curr_width < space_left && end->piece->length > end->offset + 1) {
    float next_rune_width = positions.positions[end->offset + 1].maxx - positions.positions[end->offset + 1].minx;
    if (curr_width + next_rune_width < space_left) {
      // Next rune fits
      end->offset++;
      curr_width += next_rune_width;
    } else {
      // Next rune does not fit
      break;
    }
  }
  *space_occupied = curr_width;
  *row_full = end->piece->length - 1 != end->offset;
}

// Calculate how the lines of an editable text should be broken up
static void break_lines(AppContext *app, editable_text_t *conf, vec_t(editable_line_t) *lines, float width, vec_t(piece_rune_positions_t) *piece_rune_positions) {
  // Calculate rune positions for all pieces
  piecetable_piece_t *curr_piece = conf->first_piece;
  while (curr_piece) {
    rune_position_t *positions_arr = arenaalloc(&app->ops_arena, sizeof(rune_position_t) * curr_piece->length);
    assert(positions_arr != NULL);
    piece_rune_positions_t positions = {
      .length = curr_piece->length,
      .positions = positions_arr,
    };

    const rune_t *source = curr_piece->from_original ? conf->ed->original : conf->ed->added;
    rune_text_positions(
      app->vg,
      conf->font_size,
      conf->font,
      source + curr_piece->start,
      source + curr_piece->start + curr_piece->length,
      positions.positions,
      curr_piece->length
    );
    vecpush(*piece_rune_positions, positions);
    curr_piece = curr_piece->next;
  }

  // Break into lines
  float remaining_row_width = width;
  pieceposition_t current_line_start = {
    .piece = conf->first_piece,
    .offset = 0,
  };
  pieceposition_t current_line_end = current_line_start;

  pieceposition_t curr_rune = current_line_start; // The next rune to be handled
  int piece_index = 0; // Index of the current piece in the piece_rune_positions array

  while (1) {
    pieceposition_t part_end;
    bool row_full = false;
    float part_width = 0;
    find_longest_fitting_piece_part(
      curr_rune,
      (*piece_rune_positions)[piece_index],
      remaining_row_width,
      &part_end,
      &row_full,
      &part_width
    );
    current_line_end = part_end;

    if (row_full) {
      // New row
      // The current piece has not been fully consumed yet

      // Add the line
      editable_line_t line = (editable_line_t){
        .start = current_line_start,
        .end = current_line_end,
      };
      debug_log("Filled line %lu\n", veclen(*lines));
      vecpush(*lines, line);
      debug_log("Added line from %p %d to %p %d\n", (void *) line.start.piece, line.start.offset, (void *) line.end.piece, line.end.offset);
      curr_rune = (pieceposition_t){
        .piece = part_end.piece,
        .offset = part_end.offset + 1,
      };

      current_line_start = curr_rune;
      current_line_end = current_line_start;
      remaining_row_width = width;
    } else {
      // The current piece has been fully consumed
      remaining_row_width -= part_width;
      current_line_end = part_end;
      debug_log("Added piece to non-full line %lu\n", veclen(*lines));
       
      // Next piece
      if (part_end.piece->next) {
        curr_rune = (pieceposition_t){
          .piece = part_end.piece->next,
          .offset = 0,
        };
        piece_index++;
      } else {
        // No more pieces, were done.
        // Add the last line
        editable_line_t line = (editable_line_t){
          .start = current_line_start,
          .end = current_line_end,
        };
        debug_log("Filled last line %lu in text\n", veclen(*lines));
        vecpush(*lines, line);
        debug_log("Added line from %p %d to %p %d\n", (void *) line.start.piece, line.start.offset, (void *) line.end.piece, line.end.offset);

        break;
      }
    }
  }
}

point_t editable_text(AppContext *app, point_t constraints, editable_text_t *conf) {
  vec_t(editable_line_t) lines = vec(editable_line_t, 16);
  vec_t(piece_rune_positions_t) rune_text_positions = vec(piece_rune_positions_t, 16);
  vec_t(editable_piece_part_t) parts = vec(editable_piece_part_t, 16);
  vec_t(element_t) part_elements = vec(element_t, 16);

  break_lines(app, conf, &lines, constraints.x, &rune_text_positions);

  debug_log("Broke text into %lu lines\n", veclen(lines));

  element_t line_elements[veclen(lines)];
  row_t rows[veclen(lines)];

  int curr_piece_index = 0;

  for (int i = 0; i < veclen(lines); i++) {
    int line_first_part_index = veclen(parts);
    pieceposition_t curr_position = lines[i].start;
    while (curr_position.piece && curr_position.piece != lines[i].end.piece) {
      editable_piece_part_t part = {
        .font = conf->font,
        .font_size = conf->font_size,
        .color = conf->color,
        .cursor = conf->cursor,
        .ed = conf->ed,

        .piece = curr_position.piece,
        .piece_rune_positions = rune_text_positions[curr_piece_index],
        .start = curr_position.offset,
        .length = curr_position.piece->length - curr_position.offset,
      };
      vecpush(parts, part);
      element_t part_element = {
        .widget = {
          .draw = (widget_draw_t) editable_piece_part,
          .data = &parts[veclen(parts) - 1],
        }
      };
      vecpush(part_elements, part_element);

      curr_position = (pieceposition_t){
        .piece = curr_position.piece->next,
        .offset = 0,
      };
      curr_piece_index++;
    }
    // Rendering the last piece of the line
    editable_piece_part_t part = {
      .font = conf->font,
      .font_size = conf->font_size,
      .color = conf->color,
      .cursor = conf->cursor,
      .ed = conf->ed,

      .piece = curr_position.piece,
      .piece_rune_positions = rune_text_positions[curr_piece_index],
      .start = curr_position.offset, 
      .length = lines[i].end.offset - curr_position.offset,
    };
    vecpush(parts, part);
    element_t part_element = {
      .widget = {
        .draw = (widget_draw_t) editable_piece_part,
        .data = &parts[veclen(parts) - 1],
      }
    };
    vecpush(part_elements, part_element);

    rows[i] = (row_t){
      .children = (element_children_t){
        .elements = part_elements + line_first_part_index,
        .count = veclen(part_elements) - line_first_part_index,
      },
    };

    line_elements[i] = (element_t){
      .width = {100, unit_percent},
      .widget = {
        .draw = (widget_draw_t) row,
        .data = &rows[i],
      },
    };
  }

  point_t dimensions = column(app, constraints, &(row_t){
    .spacing  = 6,
    .children = (element_children_t){
      .count    = veclen(lines),
      .elements = line_elements,
    }
  });

  // Cleanup
  vecfree(lines);
  vecfree(rune_text_positions);
  vecfree(parts);
  vecfree(part_elements);

  return dimensions;
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
          pthread_create(&file_open_thread, NULL, (void *(*)(void *)) editor_open_file, (void *) ed);
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

  element_t block_elements[block_count];

  block_t *curr_block = ed->first;
  int block_index = 0;
  while (curr_block) {
    block_draw_data_t *draw_data = arenaalloc(&app->ops_arena, sizeof(block_draw_data_t));
    *draw_data = (block_draw_data_t){
      .ed = ed,
      .block = curr_block,
    };
    widget_t block_widget = (widget_t){
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
    .overflow_handling = overflow_handling_scroll,
    .scroll_offset = scroll_offset,
    .children = (element_children_t){
      .count = block_count,
      .elements = block_elements,
    },
  });


  return constraints;
}

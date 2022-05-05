// Functions on the editor directly

#define _DEFAULT_SOURCE
#include "editor_ops.h"
#include "block_ops.h"
#include "piece_ops.h"
#include "rune_buffers.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

// Check whether the editor is empty. If it is, insert an empty paragraph block.
// This is done to prevent the the user from removing all blocks,
// at which point there is no way to add content again.
void editor_ensure_not_empty(editor_t *ed) {
  if (!ed->first) {
    piecetable_piece_t *bt = editor_create_new_blockterminator(ed);
    block_t *paragraph = (block_t *) editor_create_block_paragraph(ed, bt, bt);
    ed->first = paragraph;
    ed->last = paragraph;
    ed->cursor = (editor_cursor_t){
      .block = paragraph,
      .piece = bt,
      .offset = 0,
    };
  }
}

void editor_insert_before(editor_t *ed, editor_cursor_t *cursor, rune_t rune) {
  if (cursor->offset == 0) {
    // At the start of a piece, no split is required. Simply append to a piece infront.
    piecetable_piece_t *append_piece = cursor->piece->prev;
    if (!piece_can_append(ed, append_piece)) {
      // Previous piece can't be appended to, insert a new one.
      append_piece = insert_piece_before(cursor->block, cursor->piece, false, veclen(ed->added), 0);
    }
    append_rune(ed, rune);
    append_piece->length++;
  } else {
    // Cursor inside a piece, split required
    piecetable_piece_t *start_piece = cursor->piece;

    piecetable_piece_t *middle_piece = (piecetable_piece_t*) malloc(sizeof(piecetable_piece_t));

    piecetable_piece_t *end_piece = (piecetable_piece_t*) malloc(sizeof(piecetable_piece_t));
    *end_piece = *cursor->piece;
    end_piece->start += cursor->offset;
    end_piece->length -= cursor->offset;

    start_piece->length = cursor->offset;

    *middle_piece = (piecetable_piece_t){
      .from_original = false,
      .start = veclen(ed->added),
      .length = 1,
    };
    append_rune(ed, rune);

    // Add next linking
    end_piece->next = cursor->piece->next;
    middle_piece->next = end_piece;
    start_piece->next = middle_piece;

    // Add prev linking
    if (end_piece->next) {
      end_piece->next->prev = end_piece;
    } else {
      // If the cursor was on the last piece in this block, the end_piece is the new last piece
      cursor->block->last_piece = end_piece;
    }
    end_piece->prev = middle_piece;
    middle_piece->prev = start_piece;

    // Adjust cursor
    cursor->piece = end_piece;
    cursor->offset = 0;
  }

}

void editor_check_health(editor_t *ed) {
  block_t *curr_block = ed->first;
  while (curr_block) {
    editor_block_check_health(curr_block);
    curr_block = curr_block->next;
  }
}

void editor_delete_backwards(editor_t *ed, editor_cursor_t *cursor) {
  if (cursor->offset == 0) {
    if (cursor->piece->prev) {
      cursor->piece->prev->length--;
      piece_gc(cursor->block, cursor->piece->prev);
    } else {
      // Delete at the beginning of a block

      if (cursor->block->type != blocktype_paragraph) {
        // If the block is not a paragraph, turn it into one
        block_t *paragraph = (block_t *) editor_create_block_paragraph(ed, NULL, NULL);
        editor_block_turn_into(ed, cursor->block, paragraph);
        cursor->block = paragraph;
      } else {
        // If the block is a paragraph, append its content to the previous block
        if (cursor->block->prev) {
          // Only do this if this is not the first block
          block_append_pieces(
            ed,
            cursor->block->prev,
            cursor->block->first_piece,
            cursor->block->last_piece
          );

          block_t *old_block = cursor->block;
          cursor->block = cursor->block->prev;
          editor_delete_block(ed, old_block);
        }
      }
    }
  } else if (cursor->offset == 1) {
    cursor->piece->start++;
    cursor->piece->length--;
    cursor->offset = 0;
    piece_gc(cursor->block, cursor->piece);
  } else {
    // Split
    piecetable_piece_t *end_piece = insert_piece_after(
      cursor->block,
      cursor->piece,
      cursor->piece->from_original,
      cursor->piece->start + cursor->offset,
      cursor->piece->length - cursor->offset
    );
    cursor->piece->length = cursor->offset - 1;

    cursor->piece = end_piece;
    cursor->offset = 0;
  }
}

void editor_move_cursor_forward(editor_t *ed, editor_cursor_t *cursor) {
  if (cursor->offset < cursor->piece->length - 1) {
    // Shift offset in current piece
    cursor->offset++;
    return;
  }

  if (cursor->piece->next) {
    // Move to next piece
    cursor->piece = cursor->piece->next;
    cursor->offset = 0;
    return;
  }

  if (cursor->block->next) {
    // Move to next block
    cursor->block = cursor->block->next;
    cursor->piece = cursor->block->first_piece;
    cursor->offset = 0;
  }
}

void editor_move_cursor_backward(editor_t *ed, editor_cursor_t *cursor) {
  if (cursor->offset > 0) {
    // Shift offset in current piece
    cursor->offset--;
    return;
  }

  if (cursor->piece->prev) {
    // Move to previous piece
    cursor->piece = cursor->piece->prev;
    cursor->offset = cursor->piece->length - 1;
    return;
  }

  if (cursor->block->prev) {
    // Move to previous block
    cursor->block = cursor->block->prev;
    cursor->piece = cursor->block->last_piece;
    cursor->offset = cursor->piece->length - 1; // TODO: Probably remove the -1 so the cursor sits *after* the block content
  }
}

rune_t editor_cursor_rune(editor_t *ed, editor_cursor_t cursor) {
  const rune_t *source = cursor.piece->from_original ? ed->original : ed->added;
  return source[cursor.offset];
}

int editor_block_count(editor_t *ed) {
  int block_count = 0;
  block_t *curr = ed->first;
  while(curr) {
    block_count++;
    curr = curr->next;
  }
  return block_count;
}

editor_t editor_create(char *initial_content_string) {
  int rune_count = runes_decoding_length(initial_content_string);
  rune_t *original = (rune_t*) malloc(sizeof(rune_t) * rune_count);
  char *decoding_ptr = initial_content_string;
  for(int i = 0; i < rune_count; i++) {
    original[i] = rune_decode(&decoding_ptr);
  }

  editor_t editor = {
    .original = original,
    .added = vec(rune_t, 64),
    .current_file_fd = -1,
  };

  piecetable_piece_t *block_terminator = editor_create_new_blockterminator(&editor);

  block_t *original_block = (block_t*) editor_create_block_paragraph(&editor, block_terminator, block_terminator);
  editor.first = original_block;
  editor.last = original_block;

  piecetable_piece_t *original_piece = insert_piece_before((block_t *) original_block, original_block->first_piece, true, 0, rune_count);

  editor.cursor = (editor_cursor_t){
    .block = original_block,
    .piece = original_piece,
    .offset = 0,
  };

  return editor;
}

void editor_clear(editor_t *ed) {
  block_t *curr_block = ed->first;
  while (curr_block) {
    piecetable_piece_t *curr_piece = curr_block->first_piece;
    while (curr_piece) {
      piecetable_piece_t *next_piece = curr_piece->next;
      free(curr_piece);
      curr_piece = next_piece;
    }
    block_t *next_block = curr_block->next;
    free(curr_block);
    curr_block = next_block;
  }
  vecfree(ed->added);
  free(ed->original);

  ed->added = vec(rune_t, 64);
  ed->original = NULL;

  piecetable_piece_t *bt = editor_create_new_blockterminator(ed);
  block_t *paragraph = (block_t *) editor_create_block_paragraph(ed, bt, bt);
  ed->first = paragraph;
  ed->last = paragraph;

  ed->cursor = (editor_cursor_t){
    .block = paragraph,
    .piece = bt,
    .offset = 0,
  };
}

bool editor_open_file(editor_t *ed, const char *file_path) {
  int fd = open(file_path, O_RDWR);
  if (fd < 0) return false;
  if (editor_import_markdown_filedesc(ed, fd) != 0) {
    close(fd);
    return false;
  }
  ed->current_file_fd = fd;
  return true;
}

bool editor_save_file(editor_t *ed) {
  if (ed->current_file_fd < 0) return false;
  FILE *stream = fdopen(ed->current_file_fd, "r+");
  if (stream == NULL) return false;
  editor_export_markdown(ed, stream);
  fclose(stream);
  return true;
}

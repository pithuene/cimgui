#include "piecetable.h"

editor_t editor_create(char *initial_content_string) {
  int rune_count = runes_decoding_length(initial_content_string);
  rune_t *original = (rune_t*) malloc(sizeof(rune_t) * rune_count);
  char *decoding_ptr = initial_content_string;
  for(int i = 0; i < rune_count; i++) {
    original[i] = rune_decode(&decoding_ptr);
  }

  piecetable_piece_t *original_piece = (piecetable_piece_t*) malloc(sizeof(piecetable_piece_t));
  *original_piece = (piecetable_piece_t){
    .from_original = true,
    .length = (uint32_t) rune_count,
    .start = 0,
  };

  block_paragraph_t *original_block = (block_paragraph_t*) malloc(sizeof(block_paragraph_t));
  *original_block = (block_paragraph_t){
    .block = {
      .type = blocktype_paragraph,
      .first_piece = original_piece,
      .last_piece = original_piece,
    }
  };

  return (editor_t){
    .original = original,
    .added = vec(rune_t, 64),
    .first = (block_t*) original_block,
    .last  = (block_t*) original_block,
    .cursor = (editor_cursor_t){
      .block = (block_t *) original_block,
      .piece = original_piece,
      .offset = 0,
    },
  };
}

void editor_move_cursor_forward(editor_t *ed, editor_cursor_t *cursor) {
  if (cursor->offset < cursor->piece->length) {
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

  if (cursor->block->next) {
    // Move to previous block
    cursor->block = cursor->block->prev;
    cursor->piece = cursor->block->last_piece;
    cursor->offset = cursor->piece->length - 1; // TODO: Probably remove the -1 so the cursor sits *after* the block content
  }
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

#include "piecetable.h"

// Append a single rune to the added buffer
static void append_rune(editor_t *ed, rune_t rune) {
  vecpush(ed->added, rune);
}

// Append an array of runes to the added buffer
static void append_text(editor_t *ed, rune_t *runes, int rune_count) {
  for (int i = 0; i < rune_count; i++) {
    append_rune(ed, runes[i]);
  }
}

static piecetable_piece_t *create_new_blockterminator(editor_t *ed) {
  piecetable_piece_t *newline_piece = (piecetable_piece_t*) malloc(sizeof(piecetable_piece_t));
  *newline_piece = (piecetable_piece_t){
    .from_original = false,
    .length = 1,
    .start = veclen(ed->added),
  };
  append_rune(ed, '\0' << 24);
  return newline_piece;
}

// Insert a new piece after a given one inside a block.
// Returns the new piece.
static piecetable_piece_t *insert_piece_after(block_t *block, piecetable_piece_t *piece, bool from_original, uint32_t start, uint32_t length) {
  piecetable_piece_t *new_piece = (piecetable_piece_t*) malloc(sizeof(piecetable_piece_t));
  *new_piece = (piecetable_piece_t){
    .from_original = from_original,
    .length = length,
    .start = start,
  };

  /*      next
  +-----+------> +-----------+
  |piece|        |piece->next|
  +-----+ <------+-----------+
            prev            */

  new_piece->prev = piece;
  if (piece->next) {
    piece->next->prev = new_piece;
  }

  /* next
     +-------------------------------------+
     |                                     v
  +--+--+        +---------+        +-----------+
  |piece|   prev |new_piece|   prev |piece->next|
  +-----+ <------+---------+ <------+------+----+
     ^                                     |
     +-------------------------------------+
                                        prev */

  new_piece->next = piece->next;
  piece->next = new_piece;
  /*        next               next
  +--+--+------> +---------+------> +---+-------+
  |piece|        |new_piece|        |piece->next|
  +-----+ <------+---------+ <------+------+----+
            prev               prev            */

  if (piece == block->last_piece) {
    block->last_piece = new_piece;
  }

  return new_piece;
}

// Create and insert a new piece before a given one.
static piecetable_piece_t *insert_piece_before(block_t *block, piecetable_piece_t *piece, bool from_original, uint32_t start, uint32_t length) {
  piecetable_piece_t *new_piece = (piecetable_piece_t*) malloc(sizeof(piecetable_piece_t));
  *new_piece = (piecetable_piece_t){
    .from_original = from_original,
    .length = length,
    .start = start,
  };

  new_piece->next = piece;
  if (piece->prev) {
    piece->prev->next = new_piece;
  }

  new_piece->prev = piece->prev;
  piece->prev = new_piece;

  if (piece == block->first_piece) {
    block->first_piece = new_piece;
  }

  return new_piece;
}

// If a piece is not NULL and references the end of the added buffer, it can be appended to.
static bool piece_can_append(editor_t *ed, piecetable_piece_t *piece) {
  if (piece == NULL) return false;
  if (piece->from_original) return false;
  if (piece->start + piece->length != veclen(ed->added)) return false;
  return true;
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

// Garbage collection for pieces.
// If the piece is empty, it is properly removed and freed.
static void piece_gc(block_t *block, piecetable_piece_t *piece) {
  if (!piece || piece->length > 0) return;

  if (piece->prev) {
    piece->prev->next = piece->next;
  } else {
    block->first_piece = piece->next;
  }

  if (piece->next) {
    piece->next->prev = piece->prev;
  } else {
    block->last_piece = piece->prev;
  }

  /*
  if (!block->first_piece) {
    // Block is empty
    // TODO
  }
  */
  free(piece);
}

void editor_delete_backwards(editor_t *ed, editor_cursor_t *cursor) {
  if (cursor->offset == 0) {
    if (cursor->piece->prev) {
      cursor->piece->prev->length--;
      piece_gc(cursor->block, cursor->piece->prev);
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
      cursor->offset,
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
  };

  piecetable_piece_t *block_terminator = create_new_blockterminator(&editor);

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

block_paragraph_t *editor_create_block_paragraph(editor_t *ed, piecetable_piece_t *first, piecetable_piece_t *last) {
  block_paragraph_t *new_paragraph = (block_paragraph_t*) malloc(sizeof(block_paragraph_t));
  *new_paragraph = (block_paragraph_t){
    .block = {
      .type = blocktype_paragraph,
      .first_piece = first,
      .last_piece = last,
    }
  };
  return new_paragraph;
}

void editor_insert_block_after(editor_t *ed, block_t *after, block_t *new_block) {
  if (after->next) {
    after->next->prev = new_block;
    new_block->next = after->next;
  } else {
    ed->last = new_block;
  }
  after->next = new_block;
  new_block->prev = after;
}

// Split the piece under a cursor into two.
// The piece is split at the character *before* the cursor.
//
// If the cursor is at the beginning of a piece, no split is performed, and first is set the previous piece.
// Note that first can be set to NULL in this case.
static void split_piece_at_cursor(editor_cursor_t *cursor, piecetable_piece_t **first, piecetable_piece_t **second) {
  if (cursor->offset == 0) {
    *second = cursor->piece;
    *first = cursor->piece->prev;
  }

  int second_piece_length = cursor->piece->length - cursor->offset;
  *first = cursor->piece;
  (*first)->length = cursor->offset;
  *second = insert_piece_after(
    cursor->block,
    cursor->piece,
    cursor->piece->from_original,
    cursor->piece->start + cursor->piece->length,
    second_piece_length
  );
}

void editor_split_block_at_cursor(editor_t *ed, editor_cursor_t *cursor) {
  // TODO: The created block should usually be of the same type. To implement this I would need some sort of a "copy block of any type" mechanism or change how blocktypes are handled.
  
  // The last content piece that remains in the cursor block
  piecetable_piece_t *first = NULL;
  // The first piece of the new block 
  piecetable_piece_t *second = NULL;

  split_piece_at_cursor(cursor, &first, &second);

  // Create and insert the new block
  second->prev = NULL;
  block_t *new_paragraph = (block_t *) editor_create_block_paragraph(ed, second, cursor->block->last_piece);
  editor_insert_block_after(ed, cursor->block, new_paragraph);

  // Append a new block_terminator to the first block and fix all links
  piecetable_piece_t *first_blockterminator = create_new_blockterminator(ed);
  first_blockterminator->prev = first;
  cursor->block->last_piece = first_blockterminator;
  first->next = first_blockterminator;

  // Place cursor at beginning of new block
  *cursor = (editor_cursor_t){
    .block = new_paragraph,
    .piece = second,
    .offset = 0,
  };
}

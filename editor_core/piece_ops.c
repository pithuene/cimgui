// Functions mainly operating on pieces
#include "piece_ops.h"
#include "rune_buffers.h"

bool piece_is_blockterminator(editor_t *ed, piecetable_piece_t *piece) {
  if (piece->from_original) return false;
  if (piece->length != 1) return false;
  if (ed->added[piece->start] != 0x000000) return false;
  return true;
}

piecetable_piece_t *editor_create_new_blockterminator(editor_t *ed) {
  piecetable_piece_t *newline_piece = (piecetable_piece_t*) malloc(sizeof(piecetable_piece_t));
  *newline_piece = (piecetable_piece_t){
    .from_original = false,
    .length = 1,
    .start = veclen(ed->added),
  };
  append_rune(ed, '\0' << 24);
  return newline_piece;
}

bool piece_can_append(editor_t *ed, piecetable_piece_t *piece) {
  if (piece == NULL) return false;
  if (piece->from_original) return false;
  if (piece->start + piece->length != veclen(ed->added)) return false;
  return true;
}

// Garbage collection for pieces.
// If the piece is empty, it is properly removed and freed.
void piece_gc(block_t *block, piecetable_piece_t *piece) {
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

// Split the piece under a cursor into two.
// The piece is split at the character *before* the cursor.
//
// If the cursor is at the beginning of a piece, no split is performed, and first is set the previous piece.
// Note that first can be set to NULL in this case.
void split_piece_at_cursor(editor_cursor_t *cursor, piecetable_piece_t **first, piecetable_piece_t **second) {
  if (cursor->offset == 0) {
    *second = cursor->piece;
    *first = cursor->piece->prev;
    return;
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

piecetable_piece_t *insert_piece_after(block_t *block, piecetable_piece_t *piece, bool from_original, uint32_t start, uint32_t length) {
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

piecetable_piece_t *insert_piece_before(block_t *block, piecetable_piece_t *piece, bool from_original, uint32_t start, uint32_t length) {
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

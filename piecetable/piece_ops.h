#ifndef _EDITOR_PIECES_H
#define _EDITOR_PIECES_H

#include <stdbool.h>
#include <stdint.h>
#include "types.h"

bool piece_is_blockterminator(editor_t *ed, piecetable_piece_t *piece);
piecetable_piece_t *editor_create_new_blockterminator(editor_t *ed);

// If a piece is not NULL and references the end of the added buffer, it can be appended to.
bool piece_can_append(editor_t *ed, piecetable_piece_t *piece);

// Garbage collection for pieces.
// If the piece is empty, it is properly removed and freed.
void piece_gc(block_t *block, piecetable_piece_t *piece);

// Split the piece under a cursor into two.
// The piece is split at the character *before* the cursor.
//
// If the cursor is at the beginning of a piece, no split is performed, and first is set the previous piece.
// Note that first can be set to NULL in this case.
void split_piece_at_cursor(editor_cursor_t *cursor, piecetable_piece_t **first, piecetable_piece_t **second);

// Insert a new piece after a given one inside a block.
// Returns the new piece.
piecetable_piece_t *insert_piece_after(block_t *block, piecetable_piece_t *piece, bool from_original, uint32_t start, uint32_t length);

// Create and insert a new piece before a given one.
piecetable_piece_t *insert_piece_before(block_t *block, piecetable_piece_t *piece, bool from_original, uint32_t start, uint32_t length);

#endif

#ifndef _EDITOR_BLOCKS_H
#define _EDITOR_BLOCKS_H

#include "types.h"
#include <stdbool.h>

// Append the pieces from first to last to the end of block.
// Automatically removes and adds the correct block terminators as necessary.
void block_append_pieces(editor_t *ed, block_t *block, piecetable_piece_t *first, piecetable_piece_t *last);

// Remove the blockterminator from a block if it has one.
// Returns whether a terminator was removed.
bool block_remove_terminator(editor_t *ed, block_t *block);

// Insert new_block after a given block
void editor_insert_block_after(editor_t *ed, block_t *after, block_t *new_block);

// Delete a block from the editor.
// Does not delete the pieces it contains!
void editor_delete_block(editor_t *ed, block_t *block);

// A block is considered empty if contains no pieces or a single blockterminator piece.
bool editor_block_is_empty(editor_t *ed, block_t *block);

// Split a block into two at the cursor
void editor_split_block_at_cursor(editor_t *ed, editor_cursor_t *cursor);

// Debugging only.
// Check if the piece list inside a block is in a healthy state.
// Check if the first_piece has no prev and the last piece has no next.
void editor_block_check_health(block_t *block);

// Change the type of a block by replacing it with another one.
// The piecetable pieces from the old block are put into the new one,
// the new block is inserted where the old block was
// and the old block is freed.
// The new block should therefore not be linked into the block list yet.
void editor_block_turn_into(editor_t *ed, block_t *old, block_t *new_block);

/*
 * Creation functions for different block types
 * Create instances of new blocks.
 * The blocks must still be linked into the list afterwards.
 */

block_heading_t *editor_create_block_heading(editor_t *ed, uint8_t level, piecetable_piece_t *first, piecetable_piece_t *last);
block_bullet_t *editor_create_block_bullet(editor_t *ed, uint8_t indentation_level, piecetable_piece_t *first, piecetable_piece_t *last);
block_paragraph_t *editor_create_block_paragraph(editor_t *ed, piecetable_piece_t *first, piecetable_piece_t *last);

// Copy a block of any type.
block_t *editor_copy_block(block_t *block);

#endif

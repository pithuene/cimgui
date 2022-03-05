#ifndef _PIECETABLE_H
#define _PIECETABLE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../font/utf8/utf8.h"
#include "../ds/ds/vec.h"

typedef struct piecetable_piece_t {
  uint32_t                   from_original : 1;
  uint32_t                   length : 31;
  uint32_t                   start;
  struct piecetable_piece_t *next;
  struct piecetable_piece_t *prev;
} piecetable_piece_t;

typedef enum {
  blocktype_paragraph,
} blocktype_t;

typedef struct block_t {
  blocktype_t type;
  // Blocks form a doubly linked list
  struct block_t *next;
  struct block_t *prev;
  // The piecetable pieces contained in this block
  piecetable_piece_t *first_piece;
  piecetable_piece_t *last_piece;
} block_t;

typedef struct {
  block_t block;
} block_paragraph_t;

typedef struct {
  block_t            *block;
  piecetable_piece_t *piece;
  uint32_t            offset;
} editor_cursor_t;

typedef struct {
  rune_t       *original;
  vec_t(rune_t) added;
  block_t *first;
  block_t *last;
  editor_cursor_t cursor;
} editor_t;

editor_t editor_create(char *initial_content_string);
// How many blocks are there in the document
int editor_block_count(editor_t *ed);
// Move a cursor forward
void editor_move_cursor_forward(editor_t *ed, editor_cursor_t *cursor);
// Move a cursor backward
void editor_move_cursor_backward(editor_t *ed, editor_cursor_t *cursor);
// Insert a rune in front of a cursor
void editor_insert_before(editor_t *ed, editor_cursor_t *cursor, rune_t rune);
// Delete the rune before a cursor
void editor_delete_backwards(editor_t *ed, editor_cursor_t *cursor);
// Insert new_block after a given block
void editor_insert_block_after(editor_t *ed, block_t *after, block_t *new_block);
// Create a new paragraph block. The block must still be linked into the list.
block_paragraph_t *editor_create_block_paragraph(editor_t *ed, piecetable_piece_t *first, piecetable_piece_t *last);
// Split a block into two at the cursor
void editor_split_block_at_cursor(editor_t *ed, editor_cursor_t *cursor);

#endif

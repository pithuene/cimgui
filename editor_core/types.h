// Types shared across the editor implementation

#ifndef _EDITOR_TYPES_H
#define _EDITOR_TYPES_H

#include <stdint.h>
#include <stdio.h>
#include "../ds/ds/vec.h"
#include "../font/utf8/utf8.h"

typedef struct piecetable_piece_t {
  uint32_t                   from_original : 1;
  uint32_t                   length : 31;
  uint32_t                   start;
  struct piecetable_piece_t *next;
  struct piecetable_piece_t *prev;
} piecetable_piece_t;

typedef enum {
  blocktype_paragraph,
  blocktype_heading,
  blocktype_bullet,
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
  block_t block;
  uint8_t level;
} block_heading_t;

typedef struct {
  block_t block;
  uint8_t indentation_level;
} block_bullet_t;

typedef struct {
  block_t            *block;
  piecetable_piece_t *piece;
  uint32_t            offset;
} editor_cursor_t;

typedef struct {
  rune_t *original;
  vec_t(rune_t) added;
  block_t *first;
  block_t *last;
  editor_cursor_t cursor;
  FILE *current_file;
} editor_t;

#endif

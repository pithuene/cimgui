#ifndef _PIECETABLE_H
#define _PIECETABLE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../ds/ds/vec.h"

// A unicode codepoint
typedef uint32_t rune_t;

rune_t rune_decode(char **input);
void rune_encode(char **output, rune_t rune);
int runes_encoding_length(rune_t *runes, int length);
int runes_decoding_length(char *string);

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
} block_t;

typedef struct {
  block_t block;
  piecetable_piece_t *first;
  piecetable_piece_t *last;
} block_paragraph_t;

typedef struct {
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
// Move a cursor forward a given distance
void editor_move_cursor_forward(editor_t *ed, editor_cursor_t *cursor, int distance);
// Move a cursor backward a given distance
void editor_move_cursor_backward(editor_t *ed, editor_cursor_t *cursor, int distance);

#endif
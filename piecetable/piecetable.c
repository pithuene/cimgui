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
    .block.type = blocktype_paragraph,
    .first = original_piece,
    .last = original_piece,
  };

  return (editor_t){
    .original = original,
    .added = vec(rune_t, 64),
    .first = (block_t*) original_block,
    .last  = (block_t*) original_block,
  };
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

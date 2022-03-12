#include "piecetable.h"

typedef void (*block_print_t)(editor_t *, block_t *, FILE *);

static void print_pieces(editor_t *ed, piecetable_piece_t *first, piecetable_piece_t *last, FILE *output) {
  piecetable_piece_t *curr_piece = first;
  char encoded[4] = {0};
  while (curr_piece) {
    rune_t *source = (curr_piece->from_original)
      ? ed->original
      : ed->added;
    for (int i = 0; i < curr_piece->length; i++) {
      const int index = curr_piece->start + i;
      char *encoding_ptr = encoded;
      rune_encode(&encoding_ptr, source[index]);
      fprintf(output, "%s", encoded);
    }
    curr_piece = curr_piece->next;
    if (curr_piece == last) break;
  }
}

static void print_paragraph(editor_t *ed, block_paragraph_t *block, FILE *output) {
  print_pieces(ed, block->block.first_piece, block->block.last_piece, output);
  fprintf(output, "\n");
}

static void print_heading(editor_t *ed, block_heading_t *block, FILE *output) {
  for (uint8_t i = 0; i < block->level; i++) {
    fprintf(output, "#");
  }
  fprintf(output, " ");
  print_pieces(ed, block->block.first_piece, block->block.last_piece, output);
  fprintf(output, "\n");
}

static void print_bullet(editor_t *ed, block_bullet_t *block, FILE *output) {
  for (uint8_t i = 0; i < block->indentation_level; i++) {
    fprintf(output, "\t");
  }
  fprintf(output, "- ");
  print_pieces(ed, block->block.first_piece, block->block.last_piece, output);
  if (!block->block.next || block->block.next->type != blocktype_bullet) {
    fprintf(output, "\n");
  }
}

static block_print_t print_function_for_type(blocktype_t type) {
  switch (type) {
    case blocktype_heading: return (block_print_t) print_heading;
    case blocktype_bullet: return (block_print_t) print_bullet;
    case blocktype_paragraph: // Fallthrough
    default: return (block_print_t) print_paragraph;
  }
}

void editor_export_markdown(editor_t *ed, FILE *output) {
  block_t *curr_block = ed->first;
  while (curr_block) {
    block_print_t print_function = print_function_for_type(curr_block->type);
    (*print_function)(ed, curr_block, output);
    curr_block = curr_block->next;
    fprintf(output, "\n");
  }
}

#include <stdio.h>
#include "piecetable.h"
#include "md4c/src/md4c.h"
#include "../font/utf8/utf8.h"

int parser_text(MD_TEXTTYPE type, MD_CHAR *text, MD_SIZE size, editor_t *ed) {
  char *decoding_ptr = text;
  while (decoding_ptr < text + size) {
    rune_t rune = rune_decode(&decoding_ptr);
    editor_insert_before(ed, &ed->cursor, rune);
  }

  return 0;
}

int parser_enter_block(MD_BLOCKTYPE type, void* detail, editor_t *ed) {
  if (type == MD_BLOCK_DOC) return 0;

  block_t *new_block;
  if (type == MD_BLOCK_H) {
    MD_BLOCK_H_DETAIL *h_detail = detail;
    new_block = (block_t *) editor_create_block_heading(
      ed,
      h_detail->level,
      NULL,
      NULL
    );
  } else {
    new_block = (block_t *) editor_create_block_paragraph(
      ed,
      NULL,
      NULL
    );
  }
  
  piecetable_piece_t *bt = editor_create_new_blockterminator(ed);
  new_block->first_piece = bt;
  new_block->last_piece = bt;

  editor_insert_block_after(ed, ed->cursor.block, new_block);

  ed->cursor = (editor_cursor_t){
    .block = new_block,
    .piece = bt,
    .offset = 0,
  };

  return 0;
}

int parser_leave_block(MD_BLOCKTYPE type, void* detail, editor_t *ed) {
  return 0;
}

int parser_enter_span(MD_SPANTYPE type, void* detail, editor_t *userdata) {
  return 0;
}

int parser_leave_span(MD_SPANTYPE type, void* detail, editor_t *userdata) {
  return 0;
}

void editor_import_markdown(editor_t *ed, const char *markdown) {
  editor_cursor_t original_cursor = ed->cursor;

  ed->cursor = (editor_cursor_t){
    .block = ed->last,
    .piece = ed->last->last_piece,
    .offset = ed->last->last_piece->length - 1,
  };

  MD_PARSER parser = {
    .text = (int (*)(MD_TEXTTYPE, const MD_CHAR*, MD_SIZE, void*)) parser_text,
    .enter_block = (int (*)(MD_BLOCKTYPE, void*, void*)) parser_enter_block,
    .leave_block = (int (*)(MD_BLOCKTYPE, void*, void*)) parser_leave_block,
    .enter_span = (int (*)(MD_SPANTYPE, void*, void*)) parser_enter_span,
    .leave_span = (int (*)(MD_SPANTYPE, void*, void*)) parser_leave_span,
  };

  md_parse(markdown, strlen(markdown), &parser, ed);

  ed->cursor = original_cursor;
}


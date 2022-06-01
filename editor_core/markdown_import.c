#define _DEFAULT_SOURCE
#include <stdio.h>
#define MD4C_USE_UTF8
#include "md4c/src/md4c.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>

#include "types.h"
#include "editor_ops.h"
#include "block_ops.h"

// TODO: Import only needed for editor_create_new_blockterminator
#include "piece_ops.h"

#define MAX_LIST_INDENT 9

typedef struct {
  editor_t *ed;
  MD_BLOCKTYPE current_listtype;
  int8_t current_list_indentation_level;
  // The current list number for each indentation level
  int8_t current_list_number[MAX_LIST_INDENT];
} parser_run_t;

int parser_text(MD_TEXTTYPE type, MD_CHAR *text, MD_SIZE size, parser_run_t *run) {
  char *decoding_ptr = text;
  while (decoding_ptr < text + size) {
    rune_t rune = rune_decode(&decoding_ptr);
    editor_insert_before(run->ed, &run->ed->cursor, rune);
  }

  return 0;
}

int parser_enter_block(MD_BLOCKTYPE type, void* detail, parser_run_t *run) {
  if (type == MD_BLOCK_DOC) return 0;

  block_t *new_block;
  if (type == MD_BLOCK_H) {
    MD_BLOCK_H_DETAIL *h_detail = detail;
    new_block = (block_t *) editor_create_block_heading(
      run->ed,
      h_detail->level,
      NULL,
      NULL
    );
  } else if (type == MD_BLOCK_UL) {
    run->current_listtype = MD_BLOCK_UL;
    run->current_list_indentation_level++;
    run->current_list_number[run->current_list_indentation_level] = 1;
    return 0;
  } else if (type == MD_BLOCK_OL) {
    run->current_listtype = MD_BLOCK_OL;
    run->current_list_indentation_level++;
    run->current_list_number[run->current_list_indentation_level] = 1;
    // TODO: Add ordered list block
    if (run->current_list_indentation_level < 1) {
      new_block = (block_t *) editor_create_block_paragraph(
        run->ed,
        NULL,
        NULL
      );
    } else {
      editor_insert_before(run->ed, &run->ed->cursor, '\n' << 24);
      return 0;
    }
  } else if (type == MD_BLOCK_LI && run->current_listtype == MD_BLOCK_UL) {
    new_block = (block_t *) editor_create_block_bullet(
      run->ed,
      run->current_list_indentation_level,
      NULL,
      NULL
    );
    run->current_list_number[run->current_list_indentation_level]++;
  } else if (type == MD_BLOCK_LI && run->current_listtype == MD_BLOCK_OL) {
    // TODO: Remove once ordered list block is implemented
    if (run->current_list_number[run->current_list_indentation_level] > 1)
      editor_insert_before(run->ed, &run->ed->cursor, '\n' << 24);
    for (int i = 0; i < run->current_list_indentation_level; i++) {
      editor_insert_before(run->ed, &run->ed->cursor, ' ' << 24);
      editor_insert_before(run->ed, &run->ed->cursor, ' ' << 24);
      editor_insert_before(run->ed, &run->ed->cursor, ' ' << 24);
      editor_insert_before(run->ed, &run->ed->cursor, ' ' << 24);
    }
    editor_insert_before(run->ed, &run->ed->cursor, ('0' + run->current_list_number[run->current_list_indentation_level]) << 24);
    editor_insert_before(run->ed, &run->ed->cursor, '.' << 24);
    editor_insert_before(run->ed, &run->ed->cursor, ' ' << 24);
    run->current_list_number[run->current_list_indentation_level]++;
    return 0;
  } else if (type == MD_BLOCK_P) {
    new_block = (block_t *) editor_create_block_paragraph(
      run->ed,
      NULL,
      NULL
    );
  } else {
    fprintf(stderr, "Unknown block type %d entered during markdown import\n", type);
    assert(false && "Unknown block type entered");
    exit(1);
  }
  
  piecetable_piece_t *bt = editor_create_new_blockterminator(run->ed);
  new_block->first_piece = bt;
  new_block->last_piece = bt;

  editor_insert_block_after(run->ed, run->ed->cursor.block, new_block);

  run->ed->cursor = (editor_cursor_t){
    .block = new_block,
    .piece = bt,
    .offset = 0,
  };

  return 0;
}

int parser_leave_block(MD_BLOCKTYPE type, void* detail, parser_run_t *run) {
  if (type == MD_BLOCK_UL || type == MD_BLOCK_OL) {
    run->current_list_indentation_level--;
  }
  return 0;
}

int parser_enter_span(MD_SPANTYPE type, void* detail, parser_run_t *run) {
  if (type == MD_SPAN_EM) {
    editor_insert_before(run->ed, &run->ed->cursor, '*' << 24);
  } else if (type == MD_SPAN_STRONG) {
    editor_insert_before(run->ed, &run->ed->cursor, '*' << 24);
    editor_insert_before(run->ed, &run->ed->cursor, '*' << 24);
  } else if (type == MD_SPAN_A) {
    editor_insert_before(run->ed, &run->ed->cursor, '[' << 24);
  }
  return 0;
}

int parser_leave_span(MD_SPANTYPE type, void* detail, parser_run_t *run) {
  if (type == MD_SPAN_EM) {
    editor_insert_before(run->ed, &run->ed->cursor, '*' << 24);
  } else if (type == MD_SPAN_STRONG) {
    editor_insert_before(run->ed, &run->ed->cursor, '*' << 24);
    editor_insert_before(run->ed, &run->ed->cursor, '*' << 24);
  } else if (type == MD_SPAN_A) {
    editor_insert_before(run->ed, &run->ed->cursor, ']' << 24);
    editor_insert_before(run->ed, &run->ed->cursor, '(' << 24);
    MD_SPAN_A_DETAIL *adetail = detail;
    char href[adetail->href.size + 1];
    href[adetail->href.size] = '\0';
    strncpy(href, adetail->href.text, adetail->href.size);
    char *decoding_ptr = href;
    int rune_count = runes_decoding_length(href);
    for(int i = 0; i < rune_count; i++) {
      editor_insert_before(run->ed, &run->ed->cursor, rune_decode(&decoding_ptr));
    }
    editor_insert_before(run->ed, &run->ed->cursor, ')' << 24);
  }
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

  parser_run_t run = {
    .ed = ed,
    .current_listtype = MD_BLOCK_UL,
    .current_list_indentation_level = -1,
  };

  md_parse(markdown, strlen(markdown), &parser, &run);

  ed->cursor = original_cursor;
}

long fsize(FILE *file) {
  long old_position = ftell(file);
  fseek(file, 0L, SEEK_END); // Seek to end of file
  long file_size = ftell(file);
  fseek(file, old_position, SEEK_SET); // Go back to where you were
  return file_size;
}

int editor_import_markdown_filestream(editor_t *ed, FILE *file) {
  if (!file) {
    return 1; // Could not open file
  }

  long file_size = fsize(file);

  const char *file_content = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fileno(file), 0);
  if (file_content == MAP_FAILED) {
    return 1; // Could not map file into memory
  }

  editor_clear(ed); // Clear the previous editor content
  editor_import_markdown(ed, file_content); // Import file

  if (munmap((void *) file_content, file_size) < 0) {
    // Could not unmap the file.
    // Since this is especially bad and leaves the file open, but unusable,
    // stop execution in debug mode.
    assert(false && "Failed to unmap memory during markdown filepath import");
    return 1;
  }

  // editor_clear leaves one empty paragraph, so the editor never reaches an unusable state.
  // This removes the initial paragraph after the file content has been inserted.
  editor_delete_block(ed, ed->first);

  // Move cursor back to file start
  ed->cursor = (editor_cursor_t){
    .block = ed->first,
    .piece = ed->first->first_piece,
    .offset = 0,
  };

  return 0;
}


int editor_import_markdown_filepath(editor_t *ed, const char *file_path) {
  FILE *file = fopen(file_path, "r");
  int res = editor_import_markdown_filestream(ed, file);
  if (res > 0) return res;
  fclose(file);
  return 0;
}

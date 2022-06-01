#ifndef _EDITOR_EDITOR_OPS_H
#define _EDITOR_EDITOR_OPS_H

#include <stdbool.h>
#include <stdio.h>
#include "types.h"

void editor_ensure_not_empty(editor_t *ed);
// Insert a rune in front of a cursor
void editor_insert_before(editor_t *ed, editor_cursor_t *cursor, rune_t rune);
// For debugging. Checks that all links in the piecetable structure are healthy.
void editor_check_health(editor_t *ed);
// Delete the rune before a cursor
void editor_delete_backwards(editor_t *ed, editor_cursor_t *cursor);

// Cursor movement
void editor_move_cursor_forward(editor_t *ed, editor_cursor_t *cursor);
void editor_move_cursor_backward(editor_t *ed, editor_cursor_t *cursor);

// Return the rune under the cursor
rune_t editor_cursor_rune(editor_t *ed, editor_cursor_t cursor);

// How many blocks are there in the document
int editor_block_count(editor_t *ed);

editor_t editor_create(char *initial_content_string);

// Remove all content from an editor
void editor_clear(editor_t *ed);

/* Markdown handling */

void editor_import_markdown(editor_t *ed, const char *markdown);
// Returns 0 on success, non-zero on error
int editor_import_markdown_filestream(editor_t *ed, FILE *file);
// Returns 0 on success, non-zero on error
int editor_import_markdown_filepath(editor_t *ed, const char *file_path);
void editor_export_markdown(editor_t *ed, FILE *output);

/* Document open / save */

// Open a file in the editor.
// Returns whether the operation was successful.
bool editor_open_file(editor_t *ed, const char *file_path);
// Write open file back to disk.
// Returns whether the operation was successful.
bool editor_save_file(editor_t *ed);

#endif

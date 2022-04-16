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

// How many blocks are there in the document
int editor_block_count(editor_t *ed);

editor_t editor_create(char *initial_content_string);

// Remove all content from an editor
void editor_clear(editor_t *ed);

// Markdown handling
void editor_import_markdown(editor_t *ed, const char *markdown);
void editor_export_markdown(editor_t *ed, FILE *output);

#endif

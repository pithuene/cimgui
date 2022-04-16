#ifndef _EDITOR_RUNE_BUFFERS_H
#define _EDITOR_RUNE_BUFFERS_H

#include "types.h"

void append_rune(editor_t *ed, rune_t rune);
void append_text(editor_t *ed, rune_t *runes, int rune_count);

#endif

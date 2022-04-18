#include "rune_buffers.h"
#include "../ds/ds/vec.h"

// Append a single rune to the added buffer
void append_rune(editor_t *ed, rune_t rune) {
  vecpush(ed->added, rune);
}

// Append an array of runes to the added buffer
void append_text(editor_t *ed, rune_t *runes, int rune_count) {
  for (int i = 0; i < rune_count; i++) {
    append_rune(ed, runes[i]);
  }
}

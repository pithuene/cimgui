#ifndef _PIECETABLE_H
#define _PIECETABLE_H

#include <stdint.h>

// A unicode codepoint
typedef uint32_t rune_t;

rune_t rune_decode(char **input);
void rune_encode(char **output, rune_t rune);
int runes_encoding_length(rune_t *runes, int length);
int runes_decoding_length(char *string);

typedef struct {
  const char *original;
  char       *added;
} piecetable_t;

#endif

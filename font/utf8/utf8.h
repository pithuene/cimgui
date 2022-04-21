#ifndef _UTF8_H
#define _UTF8_H

#include <stdint.h>
#include <stdbool.h>

// A unicode codepoint
typedef uint32_t rune_t;

int rune_length(char first_byte);
rune_t rune_decode(char **input);
void rune_encode(char **output, rune_t rune);
int runes_encoding_length(const rune_t *runes, int length);
int runes_decoding_length(char *string);
bool rune_is_newline(rune_t rune);
bool rune_is_whitespace(rune_t rune);

#endif

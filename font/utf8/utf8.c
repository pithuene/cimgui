#include "utf8.h"

const rune_t RUNE_REPLACEMENT = 0xEFBFBD00;

// Return the bytelength of the codepoint starting with first_byte.
// Returns -1 for invalid values.
int rune_length(char first_byte) {
  if ((first_byte & 0x80) == 0x00) {
    return 1;
  }
  if ((first_byte & 0xE0) == 0xC0) {
    return 2;
  }
  if ((first_byte & 0xF0) == 0xE0) {
    return 3;
  }
  if ((first_byte & 0xF8) == 0xF0) {
    return 4;
  }
  return -1;
}

// Calculate how many runes there are in a string
int runes_decoding_length(char *string) {
  int rune_count = 0;
  while (*string) {
    int rune_len = rune_length(*string);
    if (rune_len < 0) {
      return -1;
    }
    rune_count++;
    string += rune_len;
  }
  return rune_count;
}

// Calculate how many bytes a runestring will be encoded into
int runes_encoding_length(const rune_t *runes, int length) {
  int byte_count = 0;
  for (int i = 0; i < length; i++) {
    int rune_len = rune_length(((runes[i] & 0xFF000000) >> 24) & 0x000000FF);
    if (rune_len < 0) {
      return -1;
    }
    byte_count += rune_len;
  }
  return byte_count;
}

rune_t rune_decode(char **input) {
  int rune_len = rune_length(**input);
  if (rune_len < 0) {
    (*input)++; // Skip invalid byte
    return RUNE_REPLACEMENT;
  }
  rune_t result = 0;

  for (int i = 0; i < rune_len; i++) {
    int shift = (3 - i) * 8;
    rune_t mask = 0xFF000000 >> (8*i);
    rune_t addition = (rune_t) **input;
    addition <<= shift;
    addition &= mask;
    result += addition;
    (*input)++;
  }

  return result;
}

void rune_encode(char **output, rune_t rune) {
  int rune_len = rune_length(((rune & 0xFF000000) >> 24) & 0x000000FF);

  for (int i = 0; i < rune_len; i++) {
    rune_t mask = 0x000000FF << (8*(3-i));
    int shift = (3 - i) * 8;
    **output = ((rune & mask) >> shift) & 0x000000FF;
    (*output)++;
  }
}

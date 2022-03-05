#include "utf8.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

int main(void) {
  char *input = "дНе смотри назад";

  // String to runes
  int rune_count = runes_decoding_length(input);
  rune_t runes[rune_count];
  char *decoding_ptr = input;
  for(int i = 0; i < rune_count; i++) {
    runes[i] = rune_decode(&decoding_ptr);
  }

  int encoding_length = runes_encoding_length(runes, rune_count);
  char encoded[encoding_length + 1];
  for(int i = 0; i < encoding_length; i++) encoded[i] = '\0';
  char *encoding_ptr = (char*) encoded;
  for(int i = 0; i < rune_count; i++) {
    rune_encode(&encoding_ptr, runes[i]);
  }

  printf("Input: '%s'\n", input);
  printf("Output: '%s'\n", encoded);

  assert(0 == memcmp(input, encoded, strlen(input)));
  return 0;
}

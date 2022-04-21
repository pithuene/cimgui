#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../../test/munit/munit.h"
#include "utf8.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

static MunitResult test_rune_length(const MunitParameter params[], void* user_data_or_fixture) {
  char one_byte[] = "a";
  char two_bytes[] = "ü";
  char three_bytes[] = "✂";
  char four_bytes[] = "🎔";
  char invalid[] = "🎔";
  invalid[0] = (char) 0xFF; // Out of unicode range

  assert_int(rune_length(*one_byte), ==, 1);
  assert_int(rune_length(*two_bytes), ==, 2);
  assert_int(rune_length(*three_bytes), ==, 3);
  assert_int(rune_length(*four_bytes), ==, 4);
  assert_int(rune_length(*invalid), ==, -1);

  return MUNIT_OK;
}

static MunitResult test_encoding_decoding(const MunitParameter params[], void* user_data_or_fixture) {
  char *input = "Hello World Не смотри назад ✂ 🎔";

  // String to runes
  int rune_count = runes_decoding_length(input);
  rune_t runes[rune_count];
  char *decoding_ptr = input;
  for(int i = 0; i < rune_count; i++) {
    runes[i] = rune_decode(&decoding_ptr);
  }

  int encoding_length = runes_encoding_length(runes, rune_count);
  char encoded[encoding_length + 1];

  for(int i = 0; i <= encoding_length; i++) encoded[i] = '\0';

  char *encoding_ptr = (char*) encoded;
  for(int i = 0; i < rune_count; i++) {
    rune_encode(&encoding_ptr, runes[i]);
  }

  assert_string_equal(input, encoded);

  return MUNIT_OK;
}

static MunitResult test_rune_types(const MunitParameter params[], void* user_data_or_fixture) {
  assert(rune_is_newline('\n' << 24));
  assert(!rune_is_newline('a' << 24));

  assert(rune_is_whitespace(' ' << 24));
  assert(!rune_is_whitespace('a' << 24));

  return MUNIT_OK;
}

static MunitResult test_invalid_encoding_decoding_length(const MunitParameter params[], void* user_data_or_fixture) {
  {
    char invalid[] = "🎔";
    invalid[0] = (char) 0xFF; // Out of unicode range
    assert_int(runes_decoding_length(invalid), ==, -1);
  }

  {
    rune_t invalid_rune = 0xFF000000;
    assert_int(runes_encoding_length(&invalid_rune, 1), ==, -1);
  }
  
  return MUNIT_OK;
}

static MunitResult test_invalid_decode(const MunitParameter params[], void* user_data_or_fixture) {
  const rune_t RUNE_REPLACEMENT = 0xEFBFBD00;
  unsigned char input[] = {0xFF};
  char *enc_ptr = (char *) input;
  assert_uint32(RUNE_REPLACEMENT, ==, rune_decode(&enc_ptr));

  return MUNIT_OK;
}

static MunitTest tests[] = {
  {"/rune_length", test_rune_length, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/encoding_decoding", test_encoding_decoding, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/invalid_decode", test_invalid_decode, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/coding_length", test_invalid_encoding_decoding_length, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {"/rune_types", test_rune_types, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

const MunitSuite utf8_test_suite = {"/font/utf8", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

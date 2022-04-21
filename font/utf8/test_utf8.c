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

static MunitResult test_utf8(const MunitParameter params[], void* user_data_or_fixture) {
  char *input = "Не смотри назад ✂ 🎔";

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

static MunitTest tests[] = {
  {
    "/rune_length", /* name */
    test_rune_length, /* test */
    NULL, /* setup */
    NULL, /* tear_down */
    MUNIT_TEST_OPTION_NONE, /* options */
    NULL /* parameters */
  },
  {
    "/utf8", /* name */
    test_utf8, /* test */
    NULL, /* setup */
    NULL, /* tear_down */
    MUNIT_TEST_OPTION_NONE, /* options */
    NULL /* parameters */
  },
  /* Mark the end of the array with an entry where the test
   * function is NULL */
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

const MunitSuite utf8_test_suite = {
  "/font/utf8", /* name */
  tests, /* tests */
  NULL, /* suites */
  1, /* iterations */
  MUNIT_SUITE_OPTION_NONE /* options */
};

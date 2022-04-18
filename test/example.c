#include "munit/munit.h"
#include "../editor_core/editor_core.h"

static MunitResult my_test(const MunitParameter params[], void* user_data_or_fixture) {
  editor_t ed = editor_create("# Title");
  
  editor_insert_before(&ed, &ed.cursor, 'a' << 24);


  munit_assert_null(ed.first->prev);

  // TODO: editor_free
  return MUNIT_OK;
}

static MunitTest tests[] = {
  {
    "/my-test", /* name */
    my_test, /* test */
    NULL, /* setup */
    NULL, /* tear_down */
    MUNIT_TEST_OPTION_NONE, /* options */
    NULL /* parameters */
  },
  /* Mark the end of the array with an entry where the test
   * function is NULL */
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite suite = {
  "/my-tests", /* name */
  tests, /* tests */
  NULL, /* suites */
  1, /* iterations */
  MUNIT_SUITE_OPTION_NONE /* options */
};

int main (int argc, const char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, (char * const*) argv);
}

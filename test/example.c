#include "munit/munit.h"
#include "../editor_core/editor_core.h"
#include "../editor_core/test_editor_core.h"
#include "../font/utf8/test_utf8.h"

int main (int argc, const char* argv[]) {
  MunitSuite suites[] = {
    editor_core_test_suite,
    utf8_test_suite,
    { NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE }
  };

  const MunitSuite top_level_suite = {
    "/cimgui", /* name */
    (MunitTest[]){
      { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
    }, /* tests */
    suites, /* suites */
    1, /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
  };
  return munit_suite_main(&top_level_suite, NULL, argc, (char * const*) argv);
}

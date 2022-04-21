#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../test/munit/munit.h"
#include "editor_core.h"
#include "piece_ops.h"
#include "types.h"

static MunitResult example_test(const MunitParameter params[], void* user_data_or_fixture) {
  editor_t ed = editor_create("# Title");
  assert_ptr_equal(ed.first, ed.last); // Only one block
  assert_int(editor_block_count(&ed), ==, 1); // editor_block_count returns the same result

  assert_int(ed.first->type, ==, blocktype_paragraph); // TODO: Initial content is not interpreted as markdown

  // There are two pieces, the initial content, and the blockterminator
  assert_ptr_equal(ed.first->first_piece, ed.first->last_piece->prev);
  assert(piece_is_blockterminator(&ed, ed.first->last_piece));

  // Move cursor
  assert_uint32(ed.cursor.offset, ==, 0);
  editor_move_cursor_backward(&ed, &ed.cursor); // No effect, already at beginning
  assert_uint32(ed.cursor.offset, ==, 0);
  editor_move_cursor_forward(&ed, &ed.cursor);
  assert_uint32(ed.cursor.offset, ==, 1);
  editor_move_cursor_forward(&ed, &ed.cursor);
  assert_uint32(ed.cursor.offset, ==, 2);
  editor_move_cursor_backward(&ed, &ed.cursor);
  assert_uint32(ed.cursor.offset, ==, 1);
  assert_uint32(editor_cursor_rune(&ed, ed.cursor), ==, ' ' << 24);

  // Clear works as expected
  editor_clear(&ed);
  assert_int(editor_block_count(&ed), ==, 1);
  assert_ptr_equal(ed.first->first_piece, ed.first->last_piece); // Only contains blockterminator

  // If last block is deleted, editor doesn't end up in an invalid empty state
  editor_delete_block(&ed, ed.first);
  assert_int(editor_block_count(&ed), ==, 1);
  assert_ptr_equal(ed.first->first_piece, ed.first->last_piece); // Only contains blockterminator

  // Import markdown
  assert_int(editor_import_markdown_filepath(&ed, "./not_a_valid_path"), ==, 1);
  assert_int(editor_block_count(&ed), ==, 1);
  assert_int(editor_import_markdown_filepath(&ed, "../editor_core/sample.md"), ==, 0);
  assert_int(editor_block_count(&ed), !=, 1);

  //editor_export_markdown(editor_t *ed, FILE *output)

  return MUNIT_OK;
}

static MunitTest tests[] = {
  {
    "/example-test", /* name */
    example_test, /* test */
    NULL, /* setup */
    NULL, /* tear_down */
    MUNIT_TEST_OPTION_NONE, /* options */
    NULL /* parameters */
  },
  /* Mark the end of the array with an entry where the test
   * function is NULL */
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

const MunitSuite editor_core_test_suite = {
  "/editor_core", /* name */
  tests, /* tests */
  NULL, /* suites */
  1, /* iterations */
  MUNIT_SUITE_OPTION_NONE /* options */
};

#define _DEFAULT_SOURCE
#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../test/munit/munit.h"
#include "editor_core.h"
#include "piece_ops.h"
#include "types.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

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

static MunitResult import_export(const MunitParameter params[], void* user_data_or_fixture) {
  // Open file
  int fd = open("../editor_core/sample.md", O_RDONLY);
  assert_int(fd, >=, 0);

  struct stat stats;
  int err = fstat(fd, &stats);
  assert_int(err, ==, 0);

  const char *file_content = mmap(NULL, stats.st_size, PROT_READ, MAP_SHARED, fd, 0);
  assert(file_content != MAP_FAILED);
  // Import the sample file
  editor_t ed = editor_create("");
  editor_clear(&ed); // Clear the previous editor content
  editor_import_markdown(&ed, file_content); // Import file
  // editor_clear leaves one empty paragraph, so the editor never reaches an unusable state.
  // This removes the initial paragraph after the file content has been inserted.
  editor_delete_block(&ed, ed.first);

  // Move cursor back to file start
  ed.cursor = (editor_cursor_t){
    .block = ed.first,
    .piece = ed.first->first_piece,
    .offset = 0,
  };

  // Export the editor state
  int fd_out = open("../editor_core/sample_out.md", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  //int fd_out = creat("../editor_core/sample_out.md", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  assert_int(fd_out, >=, 0);
  FILE *out = fdopen(fd_out, "w+");
  assert_ptr_not_null(out);
  editor_export_markdown(&ed, out);
  fclose(out);
  close(fd_out);

  // Read the output
  fd_out = open("../editor_core/sample_out.md", O_RDONLY);
  struct stat out_stats;
  err = fstat(fd_out, &out_stats);
  assert_int(err, ==, 0);
  const char *out_file_content = mmap(NULL, out_stats.st_size, PROT_READ, MAP_SHARED, fd_out, 0);
  assert(out_file_content != MAP_FAILED);

  // Compare
  assert_string_equal(file_content, out_file_content);

  assert(munmap((void *) file_content, stats.st_size) >= 0);
  assert(munmap((void *) out_file_content, out_stats.st_size) >= 0);

  // Delete temp file
  unlink("../editor_core/sample_out.md");

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
  { "/import_export", import_export, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
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

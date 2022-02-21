#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

// The memory adress at which the draw function stackframe starts.
#ifndef NDEBUG
void *draw_stack_start = NULL;
#endif

#ifndef NDEBUG
void set_draw_stack_start_internal(void *ptr) {
  draw_stack_start = ptr;
}
#endif

#ifndef NDEBUG
bool is_pointer_frame_stable(void *ptr) {
  void *check_ptr = &ptr;
  bool result = false;

  bool stack_grows_upwards;
  if ((void*)&result > (void*)&check_ptr) {
    stack_grows_upwards = true;
  } else {
    stack_grows_upwards = false;
  }

  if (stack_grows_upwards &&
     ((size_t)ptr < (size_t)draw_stack_start || (size_t)ptr > (size_t)check_ptr))
  {
    result = true;
  }

  if (!stack_grows_upwards &&
     ((size_t)ptr > (size_t)draw_stack_start || (size_t)ptr < (size_t)check_ptr))
  {
    result = true;
  }
  return result;
}
#endif

#ifndef NDEBUG
void check_tag_internal(void *tag, const char *varname, const char *filename, int line_number) {
  if (!is_pointer_frame_stable(tag)) {
    fprintf(stderr, "Unstable stack-allocated pointer '%s' used as tag in line %d of '%s'!\n", varname, line_number, filename);
    exit(1);
  }
}
#endif

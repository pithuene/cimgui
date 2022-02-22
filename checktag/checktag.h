#ifndef _H_CHECKTAG
#define _H_CHECKTAG

// The memory adress at which the draw function stackframe starts.
#ifndef NDEBUG
extern void *draw_stack_start;
void set_draw_stack_start_internal(void *);
void check_tag_internal(void *tag, const char *varname, const char *filename, int line_number);
#endif

#ifndef NDEBUG
#define check_tag(TAG) check_tag_internal(TAG, #TAG, __FILE__, __LINE__)
#else
#define check_tag(TAG) ;
#endif

#ifndef NDEBUG
#define set_draw_stack_start(PTR) set_draw_stack_start_internal(PTR)
#else
#define set_draw_stack_start(PTR) ;
#endif

#endif

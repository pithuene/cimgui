#ifndef _OPS_H
#define _OPS_H

#include "../nanovg/src/nanovg.h"
#include "../ds/mem/arenaalloc.h"
#include "../utils/utils.h"
#include "../font/font.h"
#include "../events/events.h"

// The first field in every op struct is an optype_t type,
// which specifies the type of the operation.
typedef enum {
  optype_nop,
  optype_offset,
  optype_begin_path,
  optype_fill_color,
  optype_fill,
  optype_rect,
  optype_circle,
  optype_text,
  optype_register_input_area,
} optype_t;

typedef struct {
  optype_t type;
} op_nop_t;

typedef struct {
  optype_t type;
  point_t  offset;
} op_offset_t;

typedef struct {
  optype_t type;
} op_begin_path_t;

typedef struct {
  optype_t type;
  color_t color;
} op_fill_color_t;

typedef struct {
  optype_t type;
} op_fill_t;

typedef struct {
  optype_t type;
  float    width;
  float    height;
} op_rect_t;

typedef struct {
  optype_t type;
  float    radius;
} op_circle_t;

typedef struct {
  optype_t type;
  float size;
  Font *font;
  const char *string;
  const char *end;
} op_text_t;

typedef struct {
  optype_t type;
  uint64_t area_id;
  point_t  dimensions;
} op_register_input_area_t;

typedef struct oplist_item_t {
  // Points to the operation struct.
  // First member is its type.
  optype_t *op;
  struct oplist_item_t *next;
} oplist_item_t;

typedef struct {
  arena_allocator_t *arena;
  oplist_item_t *head;
  oplist_item_t *tail;
} oplist_t;

// Create a new oplist with the same allocator as a given one
oplist_t oplist_create(oplist_t *parent);

// Append a single operation to an oplist
void oplist_append(oplist_t *oplist, optype_t *op);

// Append the entire oplist src to dest
void oplist_concat(oplist_t *dest, oplist_t *src);

// Remove all elements from the list. Doesn't free the elements.
void oplist_clear(oplist_t *oplist);

void oplist_print(oplist_t *oplist);

typedef struct {
  NVGcontext  *vg;
  point_t      offset;
  EventQueue *eventqueue;
} op_execution_state_t;

void op_execute(op_execution_state_t *state, optype_t *op);

void op_nop(oplist_t *oplist);
void op_offset(oplist_t *oplist, point_t offset);
void op_begin_path(oplist_t *oplist);
void op_fill_color(oplist_t *oplist, color_t color);
void op_fill(oplist_t *oplist);
void op_rect(oplist_t *oplist, float width, float height);
void op_circle(oplist_t *oplist, float radius);
void op_text(oplist_t *oplist, float size, Font *font, const char *string, const char *end);
// Register an input area by which input events can be filtered.
// To approximate where something will be drawn before op execution, the position it ended up at last frame is used.
// This is done by saving the information during the actual draw
// and passing it to the next one using an event.
// This means data is always one frame behind and there is no data on the first draw.
void op_register_input_area(oplist_t *oplist, point_t dimensions, uint64_t area_id);

point_t text_bounds(NVGcontext *vg, float size, Font *font, const char *string, const char *end);

typedef struct {
	const char *str;
	float x;
	float minx;
  float maxx;
} glyph_position_t;

int text_glyph_positions(NVGcontext *vg, float size, Font *font, const char *string, const char *end, glyph_position_t *positions, int max_positions);

typedef struct {
	const char* start;
	const char* end;
	const char* next_row;
	float width;
	float minx;
  float maxx;
} text_line_t;

int text_break_lines(
  NVGcontext *vg,
  Font *font,
  float font_size,
  char *content,
  char *content_end,
  float line_width,
  text_line_t* lines,
  int max_rows
);

#define with_offset(OPLIST, ...) for (         \
  int _defer_i ##__LINE__ = (op_offset((OPLIST), (__VA_ARGS__)), 0); \
  !_defer_i ## __LINE__;                \
  (_defer_i ## __LINE__ += 1), op_offset((OPLIST), point_multiply((__VA_ARGS__), -1)))

#endif

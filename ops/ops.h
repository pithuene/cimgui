#ifndef _OPS_H
#define _OPS_H

#include "../nanovg/src/nanovg.h"
#include "../ds/mem/arenaalloc.h"
#include "../utils/utils.h"
#include "../font/font.h"

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
  optype_area_mapping,
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
  point_t  source;
  bbox_t  *result;
} op_area_mapping_t;

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
  NVGcontext *vg;
  point_t     offset;
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
void op_area_mapping(oplist_t *oplist, point_t source, bbox_t *result);

point_t text_bounds(NVGcontext *vg, float size, Font *font, const char *string, const char *end);

#endif

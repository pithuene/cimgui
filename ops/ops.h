#ifndef _OPS_H
#define _OPS_H

#include "../nanovg/src/nanovg.h"
#include "../ds/mem/arenaalloc.h"
#include "../utils/utils.h"

// The first field in every op struct is an optype_t type,
// which specifies the type of the operation.
typedef enum {
  optype_nop,
  optype_rect,
} optype_t;

typedef struct {
  optype_t type;
} op_nop_t;

typedef struct {
  optype_t type;
  float    width;
  float    height;
} op_rect_t;

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

typedef struct {
  NVGcontext *vg;
  point_t     offset;
} op_execution_state_t;

void op_execute(op_execution_state_t *state, optype_t *op);

#endif

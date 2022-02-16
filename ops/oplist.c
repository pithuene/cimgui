#include "ops.h"

oplist_t oplist_create(oplist_t *parent) {
  return (oplist_t){
    .arena = parent->arena,
    .head  = NULL,
    .tail  = NULL,
  };
}

void oplist_append(oplist_t *oplist, optype_t *op) {
  oplist_item_t *item = arenaalloc(oplist->arena, sizeof(oplist_item_t*));
  *item = (oplist_item_t){
    .op = op,
    .next = NULL,
  };

  if (oplist->head == NULL) {
    // List empty
    oplist->head = item;
    oplist->tail = item;
  } else {
    // List not empty
    oplist->tail->next = item;
    oplist->tail = item;
  }
}

void oplist_concat(oplist_t *dest, oplist_t *src) {
  if (src->tail != NULL) {
    // src not empty
    dest->tail->next = src->head;
    dest->tail = src->tail;
  }
}

void oplist_clear(oplist_t *oplist) {
  oplist->head = NULL;
  oplist->tail = NULL;
}

void op_execute(op_execution_state_t *state, optype_t *op) {
  switch(*op) {
    case optype_nop: {
      return;
    }
    case optype_rect: {
      op_rect_t *op = op;
      nvgRect(state->vg,
              state->offset.x,
              state->offset.y,
              op->width,
              op->height);
      return;
    }
  }
  // TODO: Handle this more gracefully
  // UNKNOWN OP TYPE!
  exit(1);
}


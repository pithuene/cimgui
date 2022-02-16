#include "ops.h"
#include <stdio.h>

oplist_t oplist_create(oplist_t *parent) {
  return (oplist_t){
    .arena = parent->arena,
    .head  = NULL,
    .tail  = NULL,
  };
}

void oplist_append(oplist_t *oplist, optype_t *op) {
  oplist_item_t *item = arenaalloc(oplist->arena, sizeof(oplist_item_t));
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

void oplist_print(oplist_t *oplist) {
  int i = 0;
  for (oplist_item_t *item = oplist->head; item != NULL; item = item->next) {
    printf("%d: Optype %d\n", i, *item->op);
    i++;
  }
}

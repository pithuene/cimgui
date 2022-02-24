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
  printf("Oplist:\n");
  int i = 0;
  for (oplist_item_t *item = oplist->head; item != NULL; item = item->next) {
    switch(*item->op) {
      case optype_nop: {
        printf("%d: Nop\n", i);
        break;
      }
      case optype_offset: {
        op_offset_t *op = (op_offset_t*) item->op;
        printf("%d: Offset %f / %f\n", i, op->offset.x, op->offset.y);
        break;
      }
      case optype_begin_path: {
        printf("%d: BeginPath\n", i);
        break;
      }
      case optype_fill_color: {
        op_fill_color_t *op = (op_fill_color_t*) item->op;
        printf("%d: FillColor %d %d %d %d\n", i, op->color.r, op->color.g, op->color.b, op->color.a);
        break;
      }
      case optype_fill: {
        printf("%d: Fill\n", i);
        break;
      }
      case optype_rect: {
        op_rect_t *op = (op_rect_t*) item->op;
        printf("%d: Rect width: %f height: %f\n", i, op->width, op->height);
        break;
      }
      case optype_circle: {
        op_circle_t *op = (op_circle_t*) item->op;
        printf("%d: Circle radius: %f\n", i, op->radius);
        break;
      }
      case optype_text: {
        op_text_t *op = (op_text_t*) item->op;
        printf("%d: Text\n", i);
        printf("%d:   font: %s\n", i, op->font->name);
        printf("%d:   size: %f\n", i, op->size);
        if (op->end) {
          int string_len = op->end - op->string;
          printf("%d:   string: %.*s\n", i, string_len, op->string);
        } else {
          printf("%d:   string: %s\n", i, op->string);
        }
        break;
      }
      case optype_register_input_area: {
        op_register_input_area_t *op = (op_register_input_area_t*) item->op;
        printf("%d: RegisterInputArea %f / %f with id %lu\n", i, op->dimensions.x, op->dimensions.y, op->area_id);
        break;
      }
      default: {
        printf("%d: PRINT NOT IMPLEMENTED FOR OPTYPE %d\n", i, *item->op);
        break;
      }
    }
    i++;
  }
}

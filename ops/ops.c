#include "ops.h"
#include <stdio.h>

void op_execute(op_execution_state_t *state, optype_t *untyped_op) {
  switch(*untyped_op) {
    case optype_nop: {
      return;
    }
    case optype_offset: {
      op_offset_t *op = (op_offset_t*) untyped_op;
      state->offset = point_add(state->offset, op->offset);
      return;
    }
    case optype_begin_path: {
      nvgBeginPath(state->vg);
      return;
    }
    case optype_fill_color: {
      op_fill_color_t *op = (op_fill_color_t*) untyped_op;
      nvgFillColor(
        state->vg,
        nvgRGBA(
          op->color.r,
          op->color.g,
          op->color.b,
          op->color.a
        )
      );
      return;
    }
    case optype_fill: {
      nvgFill(state->vg);
      return;
    }
    case optype_rect: {
      op_rect_t *op = (op_rect_t*) untyped_op;
      nvgRect(state->vg,
              state->offset.x,
              state->offset.y,
              op->width,
              op->height);
      return;
    }
    case optype_circle: {
      op_circle_t *op = (op_circle_t*) untyped_op;
      nvgCircle(state->vg,
              state->offset.x + op->radius,
              state->offset.y + op->radius,
              op->radius);
      return;
    }
    case optype_text: {
      op_text_t *op = (op_text_t*) untyped_op;
      nvgFontSize(state->vg, op->size / op->font->heightFactor);
      nvgFontFaceId(state->vg, op->font->handle);
	    nvgTextAlign(state->vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
      nvgText(state->vg,
              state->offset.x,
              state->offset.y - (op->size * op->font->heightOffset),
              op->string,
              op->end);
      return;
    }
    case optype_area_mapping: {
      op_area_mapping_t *op = (op_area_mapping_t*) untyped_op;
      *op->result = bbox_from_dims(state->offset, op->source.x, op->source.y);
      return;
    }
  }
  // TODO: Handle this more gracefully
  // UNKNOWN OP TYPE!
  printf("Unknown optype %d\n", *untyped_op);
  exit(1);
}

void op_nop(oplist_t *oplist) {
  op_nop_t *op = arenaalloc(oplist->arena, sizeof(op_nop_t));
  op->type = optype_nop;
  oplist_append(oplist, &op->type);
}

void op_offset(oplist_t *oplist, point_t offset) {
  op_offset_t *op = arenaalloc(oplist->arena, sizeof(op_offset_t));
  op->type = optype_offset;
  op->offset = offset;
  oplist_append(oplist, &op->type);
}

void op_begin_path(oplist_t *oplist) {
  op_begin_path_t *op = arenaalloc(oplist->arena, sizeof(op_begin_path_t));
  op->type = optype_begin_path;
  oplist_append(oplist, &op->type);
}

void op_fill_color(oplist_t *oplist, color_t color) {
  op_fill_color_t *op = arenaalloc(oplist->arena, sizeof(op_fill_color_t));
  op->type = optype_fill_color;
  op->color = color;
  oplist_append(oplist, (optype_t*) op);
}

void op_fill(oplist_t *oplist) {
  op_fill_t *op = arenaalloc(oplist->arena, sizeof(op_fill_t));
  op->type = optype_fill;
  oplist_append(oplist, &op->type);
}

void op_rect(oplist_t *oplist, float width, float height) {
  op_rect_t *op = arenaalloc(oplist->arena, sizeof(op_rect_t));
  op->type = optype_rect;
  op->width = width;
  op->height = height;
  oplist_append(oplist, &op->type);
}

void op_circle(oplist_t *oplist, float radius) {
  op_circle_t *op = arenaalloc(oplist->arena, sizeof(op_circle_t));
  op->type = optype_circle;
  op->radius = radius;
  oplist_append(oplist, &op->type);
}

void op_text(oplist_t *oplist, float size, Font *font, const char *string, const char *end) {
  op_text_t *op = arenaalloc(oplist->arena, sizeof(op_text_t));
  op->type = optype_text;
  op->size = size;
  op->font = font;
  op->string = string;
  op->end = end;
  oplist_append(oplist, &op->type);
}

point_t text_bounds(NVGcontext *vg, float size, Font *font, const char *string, const char *end) {
  nvgFontSize(vg, size/font->heightFactor);
  nvgFontFaceId(vg, font->handle);
  nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
  float bounds[4];
	nvgTextBounds(vg, 0, 0, string, end, bounds);

  return (point_t){
    .x = bounds[2] - bounds[0],
    .y = size,
  };
}

// Calculate where a point is actually drawn in pixel space.
// This is done by setting the information during the actual draw,
// so data is always one frame behind and there is no data on the first draw.
void op_area_mapping(oplist_t *oplist, point_t source, bbox_t *result) {
  op_area_mapping_t *op = arenaalloc(oplist->arena, sizeof(op_area_mapping_t));
  op->type = optype_area_mapping;
  op->source = source;
  op->result = result;
  oplist_append(oplist, &op->type);
}

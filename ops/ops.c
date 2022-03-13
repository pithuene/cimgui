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
    case optype_clip: {
      op_clip_t *op = (op_clip_t*) untyped_op;
      nvgScissor(state->vg,
              state->offset.x,
              state->offset.y,
              op->width,
              op->height);
      return;
    }
    case optype_reset_clip: {
      nvgResetScissor(state->vg);
      return;
    }
    case optype_register_input_area: {
      op_register_input_area_t *op = (op_register_input_area_t*) untyped_op;
      const input_area_t area = bbox_from_dims(state->offset, op->dimensions.x, op->dimensions.y);
      eventqueue_enqueue(state->eventqueue, inputareamapping_event(op->area_id, area));
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

void op_clip(oplist_t *oplist, float width, float height) {
  op_clip_t *op = arenaalloc(oplist->arena, sizeof(op_clip_t));
  op->type = optype_clip;
  op->width = width;
  op->height = height;
  oplist_append(oplist, &op->type);
}

void op_reset_clip(oplist_t *oplist) {
  op_reset_clip_t *op = arenaalloc(oplist->arena, sizeof(op_reset_clip_t));
  op->type = optype_reset_clip;
  oplist_append(oplist, &op->type);
}

void op_register_input_area(oplist_t *oplist, point_t dimensions, uint64_t area_id) {
  op_register_input_area_t *op = arenaalloc(oplist->arena, sizeof(op_register_input_area_t));
  op->type = optype_register_input_area;
  op->dimensions = dimensions;
  op->area_id = area_id;
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

int text_glyph_positions(NVGcontext *vg, float size, Font *font, const char *string, const char *end, glyph_position_t *positions, int max_positions) {
  nvgFontSize(vg, size/font->heightFactor);
  nvgFontFaceId(vg, font->handle);
  nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
  return nvgTextGlyphPositions(vg, 0, 0, string, end, (NVGglyphPosition*) positions, max_positions);
}

int text_break_lines(
  NVGcontext *vg,
  Font *font,
  float font_size,
  char *content,
  char *content_end,
  float line_width,
  text_line_t* lines,
  int max_rows
) {
  nvgFontSize(vg, font_size / font->heightFactor);
  nvgFontFaceId(vg, font->handle);
	nvgTextAlign(vg, NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
  return nvgTextBreakLines(
    vg,
    content,
    content_end,
    line_width,
    (NVGtextRow *) lines,
    max_rows
  );
}

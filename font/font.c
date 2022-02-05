#include "font.h"
#include "../nanovg/src/nanovg.h"

Font register_font(AppContext *context, const char *name, const char *filename, float heightFactor, float heightOffset) {
  Font font = {
    .name = name,
    .heightFactor = heightFactor,
    .heightOffset = heightOffset,
  };
	font.handle = nvgCreateFont(context->vg, name, filename);
  return font;
}

void draw_text(AppContext *context, Font *font, float size, point_t pos, const char *content) {
	nvgFontSize(context->vg, (float)size/font->heightFactor);
	nvgFontFace(context->vg, font->name);
	nvgText(context->vg, pos.x, pos.y - (size * font->heightOffset), content, NULL);
}

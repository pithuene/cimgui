#ifndef _FONT_H
#define _FONT_H

#include "../application.h"

typedef struct {
  int handle;
  const char *name;
  // TODO: nvgTextMetrics might be able to automate this
  float heightFactor;
  float heightOffset;
} Font;

Font register_font(AppContext *context, const char *name, const char *filename, float heightFactor, float heightOffset);

void draw_text(AppContext *context, Font *font, float size, point_t pos, const char *content);

#endif

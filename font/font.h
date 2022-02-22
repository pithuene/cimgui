#ifndef _FONT_H
#define _FONT_H
#include "../nanovg/src/nanovg.h"

// TODO: Rename to font_t
typedef struct {
  int handle;
  const char *name;
  // TODO: nvgTextMetrics might be able to automate this
  float heightFactor;
  float heightOffset;
} Font;

Font load_font(NVGcontext *vg, char * input_pattern, float heightFactor, float heightOffset);

#endif

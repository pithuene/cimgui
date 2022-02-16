#ifndef _FONT_H
#define _FONT_H

// TODO: Rename to font_t
typedef struct {
  int handle;
  const char *name;
  // TODO: nvgTextMetrics might be able to automate this
  float heightFactor;
  float heightOffset;
} Font;

#endif

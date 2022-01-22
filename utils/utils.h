#ifndef _UTILS_H
#define _UTILS_H

#include <stdbool.h>

typedef struct {
  double x;
  double y;
} DPoint;

bool intersects_point_rect(float px, float py, float rx, float ry, float rw, float rh);

#endif

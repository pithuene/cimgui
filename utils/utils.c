#include "utils.h"

bool intersects_point_rect(float px, float py, float rx, float ry, float rw, float rh) {
  return px > rx && px < rx + rw &&
         py > ry && py < ry + rh;
}

#ifndef _UTILS_H
#define _UTILS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  uint32_t r : 8;
  uint32_t g : 8;
  uint32_t b : 8;
  uint32_t a : 8;
} color_t;

typedef struct {
  double x;
  double y;
} point_t;

typedef struct {
  // Top left corner
  point_t min;
  // Bottom right corner
  point_t max;
} bbox_t;

#ifdef IMPLEMENTATION_UTILS_H
#undef IMPLEMENTATION_UTILS_H

bool intersects_point_bbox(point_t point, bbox_t box);
bbox_t bbox_from_dims(point_t topleft, float width, float height);
point_t bbox_dims(bbox_t bbox);
float bbox_width(bbox_t bbox);
float bbox_height(bbox_t bbox);
bbox_t bbox_move(bbox_t bbox, point_t vec);
point_t point_add(point_t a, point_t b);
point_t point_multiply(point_t point, double factor);
void clamp_float(float *target, float min, float max);
void clamp_int(int *target, int min, int max);

#endif

// Checks whether a point is inside a bounding box
inline bool intersects_point_bbox(point_t point, bbox_t box) {
  return box.min.x <= point.x && point.x <= box.max.x &&
         box.min.y <= point.y && point.y <= box.max.y;
}

// Creates a bounding box from coordinates (top left) and dimensions
inline bbox_t bbox_from_dims(point_t topleft, float width, float height) {
  return (bbox_t){
    .min = topleft,
    .max = {
      .x = topleft.x + width,
      .y = topleft.y + height,
    },
  };
}

// Get the dimensions of a bounding box
inline point_t bbox_dims(bbox_t bbox) {
  return (point_t){
    .x = bbox.max.x - bbox.min.x,
    .y = bbox.max.y - bbox.min.y,
  };
}

// Get the width of a bounding box
inline float bbox_width(bbox_t bbox) {
  return bbox.max.x - bbox.min.x;
}

// Get the height of a bounding box
inline float bbox_height(bbox_t bbox) {
  return bbox.max.y - bbox.min.y;
}

// Move a bounding box by some vector
inline bbox_t bbox_move(bbox_t bbox, point_t vec) {
  return (bbox_t){
    .min = {bbox.min.x + vec.x, bbox.min.y + vec.y},
    .max = {bbox.max.x + vec.x, bbox.max.y + vec.y},
  };
}

inline point_t point_add(point_t a, point_t b) {
  return (point_t){
    .x = a.x + b.x,
    .y = a.y + b.y,
  };
}

inline point_t point_multiply(point_t point, double factor) {
  return (point_t){
    .x = point.x * factor,
    .y = point.y * factor,
  };
}

// Clamp a float value to a given range
inline void clamp_float(float *target, float min, float max) {
  if (*target < min) *target = min;
  else if (*target > max) *target = max;
}

// Clamp an int value to a given range
inline void clamp_int(int *target, int min, int max) {
  if (*target < min) *target = min;
  else if (*target > max) *target = max;
}

#endif

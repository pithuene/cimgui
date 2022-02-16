#include "element.h"

double unit_length_in_px(unit_length_t length, double container_length) {
  if (length.unit == unit_px) {
    return length.size;
  } else if (length.unit == unit_percent) {
    // TODO: Round the pixel length here?
    return container_length * (length.size / 100.0);
  } else {
    return 0;
  }
}


#include "events.h"

InputEvent nop_event(void) {
  return (InputEvent){
    .type = eventtype_nop,
  };
}

InputEvent key_event(int key, int scancode, ButtonAction action, int mods) {
  return (InputEvent){
    .type = eventtype_key,
    .instance.key = {
      .key = key,
      .scancode = scancode,
      .action = action,
      .mods = mods,
    }
  };
}

InputEvent char_event(unsigned int codepoint, int mods) {
  return (InputEvent){
    .type = eventtype_char,
    .instance.character = {
      .codepoint = codepoint,
      .mods = mods,
    }
  };
}

InputEvent mousebuttonpress_event(MouseButton button, point_t cursor, int mods) {
  return (InputEvent){
    .type = eventtype_mousebuttonpress,
    .instance.mousebuttonpress = {
      .button = button,
      .mods = mods,
      .cursor = cursor,
    }
  };
}

InputEvent mousebuttonhelddown_event(MouseButtonPressEvent press, MouseButton button, point_t cursor, int mods) {
  return (InputEvent){
    .type = eventtype_mousebuttonhelddown,
    .instance.mousebuttonhelddown = {
      .initialPress = press,
      .button = button,
      .mods = mods,
      .cursor = cursor,
    }
  };
}

InputEvent mousebuttonrelease_event(MouseButtonPressEvent press, MouseButton button, point_t cursor, int mods) {
  return (InputEvent){
    .type = eventtype_mousebuttonrelease,
    .instance.mousebuttonrelease = {
      .initialPress = press,
      .button = button,
      .mods = mods,
      .cursor = cursor,
    }
  };
}

InputEvent inputareamapping_event(uint64_t area_id, input_area_t area) {
  return (InputEvent){
    .type = eventtype_inputareamapping,
    .instance.inputareamapping = {
      .area_id = area_id,
      .mapped_area = area,
    }
  };
}

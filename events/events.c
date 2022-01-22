#include "events.h"

InputEvent nop_event(void) {
  return (InputEvent){
    .type = InputNopEvent,
  };
}

InputEvent key_event(int key, int scancode, ButtonAction action, int mods) {
  return (InputEvent){
    .type = InputKeyEvent,
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
    .type = InputCharEvent,
    .instance.character = {
      .codepoint = codepoint,
      .mods = mods,
    }
  };
}

InputEvent mousebuttonpress_event(MouseButton button, DPoint cursor, int mods) {
  return (InputEvent){
    .type = InputMouseButtonPressEvent,
    .instance.mousebuttonpress = {
      .button = button,
      .mods = mods,
      .cursor = cursor,
    }
  };
}

InputEvent mousebuttonhelddown_event(MouseButtonPressEvent press, MouseButton button, DPoint cursor, int mods) {
  return (InputEvent){
    .type = InputMouseButtonHeldDownEvent,
    .instance.mousebuttonhelddown = {
      .initialPress = press,
      .button = button,
      .mods = mods,
      .cursor = cursor,
    }
  };
}

InputEvent mousebuttonrelease_event(MouseButtonPressEvent press, MouseButton button, DPoint cursor, int mods) {
  return (InputEvent){
    .type = InputMouseButtonReleaseEvent,
    .instance.mousebuttonrelease = {
      .initialPress = press,
      .button = button,
      .mods = mods,
      .cursor = cursor,
    }
  };
}

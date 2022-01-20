#include "events.h"

InputEvent nop_event(void) {
  return (InputEvent){
    .type = InputNopEvent,
  };
}
InputEvent key_event(int key, int scancode, int action, int mods) {
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

InputEvent mousebutton_event(int button, int action, int mods) {
  return (InputEvent){
    .type = InputMouseButtonEvent,
    .instance.mousebutton = {
      .button = button,
      .action = action,
      .mods = mods,
    }
  };
}

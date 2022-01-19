#include "events.h"

InputEvent key_event(int key, int scancode, int action, int mods) {
  return (InputEvent){
    .eventType = InputKeyEvent,
    .ev.key = {
      .key = key,
      .scancode = scancode,
      .action = action,
      .mods = mods,
    }
  };
}

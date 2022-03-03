#include "events.h"
#include <stdio.h>

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

static rune_t codepoint_to_rune(const uint32_t codepoint) {
  rune_t rune = 0;
  // U+0000   U+007F    0xxxxxxx
  if( codepoint <= 0x007F ) {
    rune = codepoint << 24;
  }
  // U+0080   U+07FF    110xxxxx  10xxxxxx
  else if( codepoint <= 0x07FF ) {
    rune += (codepoint << 2) & 0x0000FF00;
    rune += (codepoint       & 0x000000FF);
    rune &= 0x00001F3F;
    rune += 0x0000C080;
    rune <<= 16;
  }
  // U+0800   U+FFFF    1110xxxx  10xxxxxx    10xxxxxx
  else if( codepoint <= 0xFFFF ) {
    rune += (codepoint << 4) & 0x00FF0000;
    rune += (codepoint << 2) & 0x0000FF00;
    rune +=  codepoint       & 0x000000FF;
    rune &= 0x000F3F3F;
    rune += 0x00E08080;
    rune <<= 8;
  }
  // U+10000  U+10FFFF  11110xxx  10xxxxxx    10xxxxxx    10xxxxxx
  else if( codepoint <= 0x10FFFF ) {
    rune += (codepoint << 6) & 0xFF000000;
    rune += (codepoint << 4) & 0x00FF0000;
    rune += (codepoint << 2) & 0x0000FF00;
    rune +=  codepoint       & 0x000000FF;
    rune &= 0x073F3F3F;
    rune += 0xF0808080;
  }
  else {
    rune = 0xEFBFBD00;
    fprintf(stderr, "codepoint_to_rune for unknown codepoint %x\n", codepoint);
  }
  return rune;
}


InputEvent char_event(rune_t codepoint) {
  return (InputEvent){
    .type = eventtype_char,
    .instance.character = {
      .rune = codepoint_to_rune(codepoint),
    }
  };
}

InputEvent charmods_event(rune_t codepoint, int mods) {
  return (InputEvent){
    .type = eventtype_charmods,
    .instance.charmods = {
      .rune = codepoint_to_rune(codepoint),
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

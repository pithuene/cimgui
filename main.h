#ifndef _H_CIMGUI_MAIN
#define _H_CIMGUI_MAIN

#include "font/font.h"
#include "widgets/widgets.h"
#include "application.h"

typedef struct {
  Font fontNormal;
  char text[200];
  int textlen;
  int fontSize;
  button_t btns[3];
} State;

#endif

#ifndef _UTILS_LOGGING_H
#define _UTILS_LOGGING_H

#include <stdio.h>

#ifdef CIMGUI_LOG
#define debug_log(TEMPLATE, ...) \
  printf("%s:%d %s ", __FILE__, __LINE__, __func__); \
  printf(TEMPLATE, __VA_ARGS__)
#else
#define debug_log(TEMPLATE, ...) ((void)0)
#endif

#endif

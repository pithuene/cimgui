#ifndef _UTILS_ASSERT_H
#define _UTILS_ASSERT_H

#include <stdio.h>
#include <assert.h>

#ifndef NDEBUG
#define ASSERT(CONDITION, ...) \
  printf("%s:%d %s ", __FILE__, __LINE__, __func__); \
  printf(TEMPLATE, __VA_ARGS__)
#else
#define debug_log(TEMPLATE, ...) ((void)0)
#endif

#endif

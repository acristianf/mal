#pragma once

#include <assert.h>

#define MEM_ALLOC(p, type)                                                     \
  {                                                                            \
    (p) = (type *)malloc(sizeof(type));                                        \
    assert((p));                                                               \
  }

#define MEM_CALLOC(p, n_items, type)                                           \
  {                                                                            \
    (p) = (type *)calloc((n_items), sizeof(type));                             \
    assert((p));                                                               \
  }

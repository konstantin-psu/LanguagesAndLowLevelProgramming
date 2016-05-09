#ifndef L4_UTCB_H
#define L4_UTCB_H
#include <l4/types.h>

static inline L4_Word_t* L4_GetUtcb() {
  L4_Word_t* utcb;
  asm volatile("  movl %%gs:0, %0\n" : "=r"(utcb));
  return utcb;
}

#endif

#ifndef L4_MESSAGE_H
#define L4_MESSAGE_H
#include <l4/types.h>
#include <l4/space.h>
#include <l4/utcb.h>

/* Message Registers: ---------------------------------------------------*/

static inline void L4_StoreMR(int i, L4_Word_t* w) {
  *w = (L4_GetUtcb())[i];
}

static inline void L4_StoreMRs(int i, int k, L4_Word_t* w) {
  for (L4_Word_t* v = L4_GetUtcb() + i; 0<k--; ) {
    *w++ = *v++;
  }
}

static inline void L4_LoadMR(int i, L4_Word_t w) {
  (L4_GetUtcb())[i] = w;
}

static inline void L4_LoadMRs(int i, int k, L4_Word_t* w) {
  for (L4_Word_t* v = L4_GetUtcb() + i; 0<k--; ) {
    *v++ = *w++;
  }
}

#endif

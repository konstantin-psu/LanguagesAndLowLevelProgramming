#ifndef L4_MISC_H
#define L4_MISC_H
#include <l4/types.h>

/* Memory Control: ------------------------------------------------------*/

EXTERNC(L4_Word_t L4_MemoryControl(L4_Word_t control,
                                  L4_Word_t attribute[4]))

#define L4_DefaultMemory    0

static inline L4_Word_t L4_Set_PageAttribute(L4_Fpage_t f,
                                             L4_Word_t attribute) {
  L4_Word_t attributes[4];
  attributes[0] = attribute;
  L4_Set_Rights(&f, L4_NoAccess);
  L4_LoadMR(0, f.raw);
  return L4_MemoryControl(0, attributes);
}

static inline L4_Word_t L4_Set_PageAttributes(L4_Word_t n,
                                              L4_Fpage_t* fpages,
                                              L4_Word_t* attributes) {
  L4_LoadMRs(0, n, (L4_Word_t*)fpages);
  return L4_MemoryControl(n-1, attributes);
}

#endif

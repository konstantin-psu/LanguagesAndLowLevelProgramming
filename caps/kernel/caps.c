/*-------------------------------------------------------------------------
 * caps.c: Operations on capability data structures 
 * Mark P Jones, Portland State University
 *-----------------------------------------------------------------------*/
#include "winio.h"		// For printf (debugging output)
#include "util.h"
#include "memory.h"
#include "caps.h"

/*-------------------------------------------------------------------------
 * Allocate memory from an untyped capability.
 */
void* alloc(struct UntypedCap* ucap, unsigned bits) {
  unsigned len   = 1<<bits;
  unsigned mask  = len-1;
  unsigned first = (ucap->next + mask) & ~mask;
  unsigned last  = first + mask;
  printf("allocating %zd bytes from ucap %x(remaining=%d), [%x-%x]\n",
           len, ucap, ((1<<ucap->bits)-ucap->next), first, last);
  if (ucap->next<=first && last<=((1<<ucap->bits)-1)) {
    unsigned* object = (unsigned*)(ucap->memory + first);
    for (unsigned i=0; i<bytesToWords(len); ++i) {
      object[i] = 0;
    }
    ucap->next = last+1;
    return (void*)object;
  }
  return 0; // Allocation failed: not enough room
}

/*-------------------------------------------------------------------------
 * Print a description of a single capability.
 */
void showCap(struct Cap* cap) {
  switch (cap->type) {
    case NullCap    :
      printf("NullCap");
      break;

    case ConsoleCap : {
      struct ConsoleCap* ccap = (struct ConsoleCap*)cap;
      printf("ConsoleCap, attr=%x", ccap->attr);
      break;
    }

    case WindowCap  : {
      struct WindowCap* wcap = (struct WindowCap*)cap;
      printf("WindowCap, window=%x, perms=%x", wcap->window, wcap->perms);
      break;
    }

    case CspaceCap  : {
      struct CspaceCap* ccap = (struct CspaceCap*)cap;
      printf("CspaceCap, cspace=%x", ccap->cspace);
      break;
    }

    case UntypedCap : {
      struct UntypedCap* ucap = (struct UntypedCap*)cap;
      unsigned           len  = (1<<ucap->bits);
      printf("UntypedCap, [%x-%x] (size=%zd), next=%x",
           ucap->memory, ucap->memory+len-1, len, ucap->next);
      break;
    }

    case PageCap  : {
      struct PageCap* pcap = (struct PageCap*)cap;
      printf("PageCap, page=%x", pcap->page);
      break;
    }

    case PageTableCap  : {
      struct PageTableCap* ptcap = (struct PageTableCap*)cap;
      printf("PageTableCap, ptab=%x", ptcap->ptab);
      break;
    }

    case TimeCap  : {
      struct TimeCap * tcap = (struct TimeCap*)cap;
      printf("TimeCap, time resolution=%x", tcap->resolution);
      break;
    }

    default         :
      printf("Unknown capability (%d)", cap->type);
  }
  putchar('\n');
}

/*-------------------------------------------------------------------------
 * Print a description of a capability space.
 */
void showCspace(struct Cspace* cspace) {
  struct Cap* cap = cspace->caps;
  int         num = 0;
  printf("  Capability space at %x\n", cspace);
  for (unsigned i=0; i<CSPACESIZE; i++) {
    if (!isNullCap(cap)) {
      printf("    0x%02x ==> ", i);
      showCap(cap);
      num++;
    }
    cap++;
  }
  printf("  %d slot(s) in use\n", num);
}

/*-----------------------------------------------------------------------*/

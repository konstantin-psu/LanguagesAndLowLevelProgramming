/*-------------------------------------------------------------------------
 * pork.h:
 * Mark P Jones, Portland State University
 *-----------------------------------------------------------------------*/
#ifndef PORK_H
#define PORK_H
#include "kip.h"
#include "simpleio.h"

#define ENTRY void        /* Marks external entry points into the kernel */
// mask/align will not work for a>=32; Intel ignores all but lower 5 bits
#define mask(e, a) ((e)&((1<<(a))-1))	/* extract lower a bits of e	 */
#define align(e,a) (((e)>>(a))<<(a))	/* clear lower a bits of e	 */

static inline unsigned min(unsigned a, unsigned b) { return (a<b) ? a : b; }
static inline unsigned max(unsigned a, unsigned b) { return (a<b) ? b : a; }

#define fromPhys(t, addr) ((t)(addr+KERNEL_SPACE))
#define toPhys(ptr)       ((unsigned)(ptr) - KERNEL_SPACE)

typedef unsigned int  bool;
typedef unsigned char byte;
extern  byte          Kip[];
extern  byte          KipEnd[];
extern  unsigned      esp0;
extern  unsigned*     utcbptr;

extern void        abortIf(bool cond, char* msg);
static inline void ASSERT(unsigned cond, char* msg) { abortIf(!cond, msg); }
extern void        halt(void);

#endif
/*-----------------------------------------------------------------------*/

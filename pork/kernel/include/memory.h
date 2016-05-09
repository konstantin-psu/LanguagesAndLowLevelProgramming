/*-------------------------------------------------------------------------
 * Page-level kernel memory allocator:
 * Mark P Jones, Portland State University
 *
 * Functions that allocate memory pages have names of the form xxxN, where
 * xxxx is a name for the function and N is an upper bound on the number of
 * pages that might be allocated during the function call.  The behavior of
 * a call to xxxxN(...) when there are fewer than N free pages is undefined.
 * Instead, callers are expected to use availPages(N) or to pass on the
 * requirement for pages in their own names.  (e.g., f2() may call g1() upto
 * two times.)  With a richer type system, we could capture this information
 * using types instead of a naming convention ...
 *-----------------------------------------------------------------------*/
#ifndef MEMORY_H
#define MEMORY_H

extern void     initMemory(void);
extern void*    allocPage1(void);
extern void     freePage(void* p);
extern bool     availPages(unsigned n);

struct Server {
  unsigned sp;
  unsigned ip;
  unsigned lo;
  unsigned hi;
};

extern struct Server Sigma0Server, Sigma1Server, RootServer;

#endif
/*-----------------------------------------------------------------------*/

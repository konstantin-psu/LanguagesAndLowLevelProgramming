#ifndef L4_SPACE_H
#define L4_SPACE_H
#include <l4/types.h>
#include <l4/thread.h>

EXTERNC(L4_Word_t L4_SpaceControl(
			L4_ThreadId_t spaceSpec,
			L4_Word_t control,
			L4_Fpage_t kipArea,
			L4_Fpage_t utcbArea,
			L4_Word_t* oldControl))

#if defined(__cplusplus)
/* This is a hack to "handle" the alternative version of
   SpaceControl that includes an additional redirector
   parameter.  (Except that, for the time being, we don't
   really handle it all; we just discard the extra parameter.)
 */
static inline L4_Word_t L4_SpaceControl(
			  L4_ThreadId_t spaceSpec,
			  L4_Word_t control,
			  L4_Fpage_t kipArea,
			  L4_Fpage_t utcbArea,
                          L4_ThreadId_t redirector,
			  L4_Word_t* oldControl) {
  return L4_SpaceControl(spaceSpec, control, kipArea, utcbArea, oldControl);
}

#endif

#endif

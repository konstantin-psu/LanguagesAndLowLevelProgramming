#ifndef L4_SCHEDULE_H
#define L4_SCHEDULE_H
#include <l4/types.h>
#include <l4/thread.h>
#include <l4/utcb.h>

/* Clock: ------------------------------------------------------------------*/

typedef union {
  L4_Word64_t raw;
  L4_Word32_t word[2];
} L4_Clock_t;

/* TODO: add ClockAdd, once I figure out type of second parameter */

static inline L4_Clock_t L4_ClockAddUsec(L4_Clock_t l, L4_Word64_t r) {
  return (L4_Clock_t){raw: l.raw+r};
}

static inline L4_Clock_t L4_ClockSubUsec(L4_Clock_t l, L4_Word64_t r) {
  return (L4_Clock_t){raw: l.raw-r};
}

static inline L4_Bool_t L4_IsClockEarlier(L4_Clock_t l, L4_Clock_t r) {
  return l.raw < r.raw;
}

static inline L4_Bool_t L4_IsClockLater(L4_Clock_t l, L4_Clock_t r) {
  return l.raw > r.raw;
}

static inline L4_Bool_t L4_IsClockEqual(L4_Clock_t l, L4_Clock_t r) {
  return l.raw == r.raw;
}

static inline L4_Bool_t L4_IsClockNotEqual(L4_Clock_t l, L4_Clock_t r) {
  return l.raw != r.raw;
}

/* Clock: (C++ Bindings) ---------------------------------------------------*/

#if defined(__cplusplus)

static inline L4_Clock_t operator+(L4_Clock_t& l, L4_Word64_t r) {
  return L4_ClockAddUsec(l, r);
}

static inline L4_Clock_t operator+(L4_Clock_t& l, L4_Clock_t r) {
  return (L4_Clock_t) {raw: l.raw + r.raw};
}

static inline L4_Clock_t operator-(L4_Clock_t& l, L4_Word64_t r) {
  return L4_ClockSubUsec(l, r);
}

static inline L4_Clock_t operator-(L4_Clock_t& l, L4_Clock_t r) {
  return (L4_Clock_t) {raw: l.raw - r.raw};
}

static inline L4_Bool_t operator<(L4_Clock_t& l, L4_Clock_t& r) {
  return L4_IsClockEarlier(l, r);
}

static inline L4_Bool_t operator>(L4_Clock_t& l, L4_Clock_t& r) {
  return L4_IsClockLater(l, r);
}

static inline L4_Bool_t operator<=(L4_Clock_t& l, L4_Clock_t& r) {
  return l.raw <= r.raw;
}

static inline L4_Bool_t operator>=(L4_Clock_t& l, L4_Clock_t& r) {
  return l.raw >= r.raw;
}

static inline L4_Bool_t operator==(L4_Clock_t& l, L4_Clock_t& r) {
  return L4_IsClockEqual(l, r);
}

static inline L4_Bool_t operator!=(L4_Clock_t& l, L4_Clock_t& r) {
  return L4_IsClockNotEqual(l, r);
}

#endif

/* System Calls: -----------------------------------------------------------*/

EXTERNC(L4_Word64_t L4_Prim_SystemClock())

static inline L4_Clock_t L4_SystemClock() {
  return (L4_Clock_t){raw:L4_Prim_SystemClock()};
}

EXTERNC(void L4_ThreadSwitch(L4_ThreadId_t dest))

static inline void L4_Yield() {
  L4_ThreadSwitch(L4_nilthread);
}

EXTERNC(L4_Word_t L4_Schedule(
			L4_ThreadId_t dest,
			L4_Word_t timesliceLen,
			L4_Word_t totQuantum,
			L4_Word_t procControl,
			L4_Word_t prio,
			L4_Word_t* remTimeslice,
			L4_Word_t* remQuantum))

static inline L4_Word_t L4_Set_Priority(L4_ThreadId_t dest, L4_Word_t prio) {
  L4_Word_t remTimeslice, remQuantum;
  return L4_Schedule(dest, (~0U), (~0U), (~0U), prio,
                      &remTimeslice, &remQuantum);
}

static inline L4_Word_t L4_Set_ProcessorNo(
                         L4_ThreadId_t dest,
                         L4_Word_t procControl) {
  L4_Word_t remTimeslice, remQuantum;
  return L4_Schedule(dest, (~0U), (~0U), procControl, (~0U),
                       &remTimeslice, &remQuantum);
}

static inline L4_Word_t L4_Timeslice(
                           L4_ThreadId_t dest,
			   L4_Word_t* addrRemTimeslice,
		   	   L4_Word_t* addrRemQuantum) {
  return L4_Schedule(dest, (~0U), (~0U), (~0U), (~0U),
                       addrRemTimeslice, addrRemQuantum);
}

static inline L4_Word_t L4_Set_Timeslice(
                           L4_ThreadId_t dest,
                           L4_Word_t timesliceLen,
                           L4_Word_t totQuantum) {
  L4_Word_t remTimeslice, remQuantum;
  return L4_Schedule(dest, timesliceLen, totQuantum, (~0U), (~0U),
                       &remTimeslice, &remQuantum);
}

/* TODO: add Set_PreemptionDelay */

#endif

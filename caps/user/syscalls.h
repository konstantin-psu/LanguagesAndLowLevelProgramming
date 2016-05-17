/*-------------------------------------------------------------------------
 * syscalls.h:
 * Mark P. Jones, Portland State University, May 2016
 *-----------------------------------------------------------------------*/
#ifndef SYSCALLS_H
#define SYSCALLS_H

/*-------------------------------------------------------------------------
 * System calls not requiring a capability argument:
 */
extern unsigned kmapPage(unsigned addr);
extern void     dump();

/*-------------------------------------------------------------------------
 * Access to the console window:
 */
#define CONSOLE  1    // default capability slot for the console

extern unsigned kputc(unsigned cap, unsigned ch);

/*-------------------------------------------------------------------------
 * Capability space manipulation:
 */
enum Capmove { MOVE=0, COPY=1 };
extern unsigned capmove(unsigned src, unsigned dst, enum Capmove copy);
extern unsigned capclear(unsigned src);

/*-------------------------------------------------------------------------
 * Allocate memory from an untyped memory area:
 */
extern unsigned allocUntyped(unsigned ucap, unsigned slot, unsigned bits);
extern unsigned allocCspace(unsigned ucap, unsigned slot);
extern unsigned allocPage(unsigned ucap, unsigned slot);
extern unsigned allocPageTable(unsigned ucap, unsigned slot);
extern unsigned remaining(unsigned ucap);

/*-------------------------------------------------------------------------
 * Mapping objects in to an address space:
 */
extern unsigned mapPage(unsigned cap, unsigned addr);
extern unsigned mapPageTable(unsigned cap, unsigned addr);

#endif
/*-----------------------------------------------------------------------*/

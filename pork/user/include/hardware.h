/*-------------------------------------------------------------------------
 * hardware.h:
 *-----------------------------------------------------------------------*/
#ifndef HARDWARE_H
#define HARDWARE_H

static inline void outb(short port, unsigned char b) {
  asm volatile("outb  %1, %0\n" : : "dN"(port), "a"(b));
}

static inline unsigned char inb(short port) {
  unsigned char b;
  asm volatile("inb %1, %0\n" : "=a"(b) : "dN"(port));
  return b;
}

#endif
/*-----------------------------------------------------------------------*/

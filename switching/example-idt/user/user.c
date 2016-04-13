/** 
 * A simple program that we will run in user mode.
 **/
#include "simpleio.h"

extern void kputc(unsigned);
extern void yield(void);
extern void hold(void);

void kputs(char* s) {
  while (*s) {
    kputc(*s++);
  }
}

void cmain() {
  int i;
  setWindow(1, 11, 47, 32);   // user process on right hand side
  cls();
  puts("in user1 code\n");
  for (i=0; i<4; i++) {
    kputs("hello, kernel1 console\n");
    puts("hello, user1 console\n");
  }
  puts("\n\nUser code does not return\n");
  yield();
  hold();
  puts("This message won't appear!\n");
}

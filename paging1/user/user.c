/* A simple program that we will run in user mode.
 */
#include "userio.h"

extern void kputc(unsigned);
extern void hold(void);

void kputs(char* s) {
  while (*s) {
    kputc(*s++);
  }
}

void cmain() {
  int i,j,k;
  int max = 600;
  //setWindow(1, 11, 47, 32);   // user process on right hand side
  setAttr(0x5f);
  cls();
  puts("in user1 code\n");
  for (i=0; i<max; i++) {
    for (j=0; j<max*max; j++) { 
        for (k=0; k<max; k++) { 
        }
    }
    kputs("hello, kernel1 console\n");
    printf("%d hello, user1 console %d\n", i, max - i);
  }
  // yield();
  puts("\n\nUser code does not return\n");
  hold();
  puts("This message won't appear!\n");
}

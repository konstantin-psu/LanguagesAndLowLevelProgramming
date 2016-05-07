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
  setAttr(0xa1);
  //setWindow(13, 11, 47, 32);   // user process on right hand side
  cls();
  puts("in user2 code\n");
  for (i=0; i<max; i++) {
    for (j=0; j<max*max; j++) {
        for (k=0; k<max; k++) { 
        }
    }
    kputs("hello, kernel2 console\n");
    printf("%d hello, user2 console %d\n", i, 600 - i);
  }
  puts("\n\nUser2 code does not return\n");
  //yield();
  hold();
  puts("This message2 won't appear!\n");
}

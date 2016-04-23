/** 
 * A simple program that we will run in user mode.
 **/
#include "simpleio.h"

extern void kputc(unsigned);
extern void yield(void);
extern void hold(void);

unsigned * flagAddr = (unsigned *)0x402604;
void kputs(char* s) {
  while (*s) {
    kputc(*s++);
  }
}

void cmain() {
  int i;
  setWindow(13, 11, 47, 32);   // user process on right hand side
  cls();
  printf("flagAddr = 0x%x\n", flagAddr);
  *flagAddr = 1234;
  puts("in user2 code\n");
  for (i=0; i<600; i++) {
    kputs("hello, kernel2 console\n");
    puts("hello, user2 console\n");
  }
  puts("\n\nUser2 code does not return\n");
  //yield();
  hold();
  puts("This message2 won't appear!\n");
}

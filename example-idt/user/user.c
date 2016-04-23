/** 
 * A simple program that we will run in user mode.
 **/
#include "simpleio.h"

extern void kputc(unsigned);
extern void yield(void);
extern void hold(void);

volatile unsigned flag = 0;
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
  printf("My flag is at 0x%x\n", &flag);
  while (flag==0) {
     /* do nothing */
  }
  printf("Somebody set my flag to %d!\n", flag);
  for (i=0; i<600; i++) {
    kputs("hello, kernel1 console\n");
    puts("hello, user1 console\n");
  }
  //yield();
  puts("\n\nUser code does not return\n");
  hold();
  puts("This message won't appear!\n");
}

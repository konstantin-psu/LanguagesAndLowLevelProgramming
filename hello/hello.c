/* ------------------------------------------------------------------------
 * hello.c:  hello, kernel world
 *
 * Mark P. Jones, February 2008
 */

/*-------------------------------------------------------------------------
 * Video RAM:
 */
#define COLUMNS         80
#define LINES           25
#define ATTRIBUTE       12
#define VIDEO           0xB8000

#include "hello.h"

typedef unsigned char single[2];
typedef single        row[COLUMNS];
typedef row           screen[LINES];

extern screen* video = (screen*)VIDEO;

/*-------------------------------------------------------------------------
 * Cursor coordinates:
 */
// static int xpos = 0;
// static int ypos = 0;


/*-------------------------------------------------------------------------
 * Output a zero-terminated string.
 */
void puts(char *msg) {
  long i = 0, j = 0;
  char out;
  while (*msg) {
    for (j = 0; j < 1; j++) {
        i = 0;
        while (i < 1000000) {i++;}
    }
    j = 0;
    out = *msg;
    msg++;
    outc(out);
  }
}

/*-------------------------------------------------------------------------
 * Main program:
 */
void hello() {
  int i;
  cls();
  setAttr((10&7)<<4);
  bootinfo();
  //puts("bootinfo is running!\n");
  for (i=0; i<1; i++) {
  puts("hhhh   hhhh\n");
    puts(" hh    hhh        lll lll\n");
    puts(" hh    hh   eeee  ll  ll   oooo\n");
    puts(" hhhhhhhh  ee  ee ll  ll  oo  oo\n");
    puts(" hh    hh eeeeeee ll  ll oo   oo\n");
    puts(" hh    hh  ee     ll  ll oo  oo\n");
    puts("hhh   hhhh  eeee  ll  ll  oooo\n");
    puts("\n");
    puts("    K e r n e l   W o r l d\n");
  }
}

/* --------------------------------------------------------------------- */

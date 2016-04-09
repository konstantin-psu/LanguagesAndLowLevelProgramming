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
#include "simpleio.h"

typedef unsigned char single1[2];
typedef single1        row1[COLUMNS];
typedef row1           screen[LINES];

extern screen* video = (screen*)VIDEO;

int rowCache[25];
/*-------------------------------------------------------------------------
 * Cursor coordinates:
 */


/*-------------------------------------------------------------------------
 * Output a zero-terminated string.
 */
void putstring(char *msg) {
  long i = 0, j = 0;
  char out;
  while (*msg) {
    for (j = 0; j < 1; j++) {       // Slow down to see how string is printed
        i = 0;
        while (i < 1000000) {i++;}
    }
    j = 0;
    out = *msg;
    msg++;
    outc(out);
  }
}


void testCoordinates() {
        if (col == 0) {     
           row--;
           if ( row < 0) {
                row = 0;
                col = 0;
           } else {
              col = rowCache[row];
           }
        } else {
            col -= 2;
        }
}


// Intention was to implement simple shell to output multiboot information
// I left it as an exercise for the future when I figure out how to implement
// malloc alternative, although it is not necessarily
void shell() {
    int k;
    char b = 0;
    while(1) {
      k = getc();          // Read input
      b = (char)k;         // Convert to char
      if (b == 0xD) {      // Test if new line
        outc('\n');        // Output new line
        rowCache[row] = 0; // reset char counter
      }
      else if (b > 0x1F && b < 0x7F) {  // test if printable char
        rowCache[row]+=2; // Output and increase count
        outc(b);
      } else if (b == 0x7F) {
        testCoordinates(); // reset by -1 position
        outc(' ');         // output space
        testCoordinates(); // reset by -1 since outc increment current col
        rowCache[row]-=2;  // decrease cache
      }
      else {
      }
    }
}

/*-------------------------------------------------------------------------
 * Main program:
 */
void hello() {
  int i;
  for (i = 0; i < 25; i++) {
    rowCache[i] = 0;
  }
  cls();
  setAttr(0x2e); // Set attribute to default value (PSU Green)
  outhex(0x5e);
  shell();
}

/* --------------------------------------------------------------------- */

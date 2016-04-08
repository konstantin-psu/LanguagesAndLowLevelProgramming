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
// static int xpos = 0;
// static int ypos = 0;


/*-------------------------------------------------------------------------
 * Output a zero-terminated string.
 */
void putstring(char *msg) {
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


void putcharr(char c){
    int i, j;

    if (c!='\n' && c!='\r') {
        (*video)[row][col][0] = c & 0xFF;
        (*video)[row][col][1] = ATTRIBUTE;
        col = col + 1;
        if (col < COLUMNS) {
            return;
        }
    }

    col = 0;               // Perform a newline
    if (++row >= LINES) {  // scroll up top lines of screen ... 
        row = LINES-1;
        for (i=0; i<row; ++i) {
          for (j=0; j<COLUMNS; ++j) {
            (*video)[i][j][0] = (*video)[i+1][j][0];
            (*video)[i][j][1] = (*video)[i+1][j][1];
          }
        }
        for (j=0; j<COLUMNS; ++j) { // fill in new blank line
          (*video)[row][j][0] = ' ';
          (*video)[row][j][1] = ATTRIBUTE;
        }
    }
}
// int compareString(char * str1, char * str2) {
//     while (*str1) {
//         if ( *str1++ != str2++ ) return 0;
//     }
//     if (*str2) return 0;
//     return 1;
// }

void testCoordinates() {
        col -= 2;
        if (col < 0) {
            row--;
            if (row < 0) row = 0;
            col = rowCache[row]*2;
        }
}
void testCoordinates2() {
        col -= 2;
        if (col < 0) {
            row--;
            if (row < 0) row = 0;
            col = 0;
        }
}
void shell() {
    int k;
    char b = 0;
    while(1) {
      k = getc();
      b = (char)k; 
      if (b == 0xD) {
        outc('\n');
        rowCache[row] = 0;
      }
      else if (b > 0x1F && b < 0x7F) {
        outc(b);
        rowCache[row]++;
      } else if (b == 0x7F) {
        testCoordinates();
        outc(' ');
        testCoordinates2();
      }
      else {
        //outc(b);
        //outc(' ');
        //outhex(b);
        //outc('\n');
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
  setAttr((10&7)<<4);
  shell();
}

/* --------------------------------------------------------------------- */

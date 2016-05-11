/*-------------------------------------------------------------------------
 * puts.c:  An implementation of capputs.
 */

#include "capio.h"

/*-------------------------------------------------------------------------
 * Output a simple string.
 */
void capputs(unsigned cap, char* s) {
  while (*s) {
    capputchar(cap, *s++);
  }
}

/*-----------------------------------------------------------------------*/

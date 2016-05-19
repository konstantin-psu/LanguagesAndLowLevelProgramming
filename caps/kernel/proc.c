/*-------------------------------------------------------------------------
 * proc.c:
 * Mark P Jones + YOUR NAME HERE, Portland State University
 *-----------------------------------------------------------------------*/
#include "winio.h"
#include "util.h"
#include "context.h"
#include "proc.h"
#include "paging.h"
#include "caps.h"

/*-------------------------------------------------------------------------
 * User-level processes:
 */
// struct Process procl;    // The set of all user-level processes
// procl.next = 0;
// procl.prev = 0;

struct Process proc[2];    // The set of all user-level processes
struct Process* current;   // Identifies the current process

void initProcess(struct Process* proc,
                 unsigned lo,
                 unsigned hi,
                 unsigned entry) {
  proc->pdir   = newUserPdir(lo, hi);
  proc->cspace = allocCspace();
  showPdir(proc->pdir);
  printf("user code is at 0x%x\n", entry);
  initContext(&proc->ctxt, entry, 0);
}

// void initProcessl(struct Process* proc,
//                  unsigned lo,
//                  unsigned hi,
//                  unsigned entry) {
//   struct Proc * current;
//   if (proc->next == 0 || proc->prev == 0) {
//       current= proc;
//       proc->next = proc;
//       proc->prev = proc;
//   }
//   current->pdir   = newUserPdir(lo, hi);
//   current->cspace = allocCspace();
//   showPdir(current->pdir);
//   printf("user code is at 0x%x\n", entry);
//   initContext(&current->ctxt, entry, 0);
// 
// }

/*-------------------------------------------------------------------------
 * Switch to the next available process:
 */
void reschedule() {
  current = (current==proc) ? (proc+1) : proc;
  setPdir(toPhys(current->pdir));
}

/*-----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Kernel Initialization and Miscellaneous Functions:
 * Mark P Jones, Portland State University
 *-----------------------------------------------------------------------*/
#include "pork.h"
#include "memory.h"
#include "space.h"
#include "threads.h"
#include "hardware.h"

ENTRY init() {
  setVideo(KERNEL_SPACE + 0xB8000);
  setAttr(0x7f);
  cls();
  extern byte KernelBanner[];
  printf("%s\n", KernelBanner);
  setWindow(1, 8, 0, 80);
  setAttr(0xd);
  cls();

  initMemory();
  initSpaces();
  initTCBs();
  startTimer();
  reschedule();
  printf("System halting\n");  // Should be unreachable
  halt();
}

/*-------------------------------------------------------------------------
 * Abort kernel operation:
 *-----------------------------------------------------------------------*/
void abortIf(bool cond, char* msg) {
  if (cond) {
    printf("%s\n", msg);
    halt();
  }
}

/*-------------------------------------------------------------------------
 * The "ProcessorControl" System Call:
 *-----------------------------------------------------------------------*/
ENTRY processorControl() { // TODO: Put this someplace else!
  if (!privileged(current->space)) {         // check for privileged thread
    retError(ProcessorControl_Result, NO_PRIVILEGE);
  } else if (ProcessorControl_ProcessorNo!=0) {               // single CPU
    // TODO: not in spec!
    retError(ProcessorControl_Result, INVALID_PARAMETER);
  } else {
    // TODO: add rest of ProcessorControl implementation here ...
    if (ProcessorControl_InternalFreq!=(-1)) {
    }
    if (ProcessorControl_ExternalFreq!=(-1)) {
    }
    if (ProcessorControl_Voltage!=(-1)) {
    }
    ProcessorControl_Result = 1;
  }
  resume();
}

/*-------------------------------------------------------------------------
 * The "MemoryControl" System Call:
 *-----------------------------------------------------------------------*/
ENTRY memoryControl() { // TODO: Put this someplace else!
  if (!privileged(current->space)) {         // check for privileged thread
    retError(ProcessorControl_Result, NO_PRIVILEGE);
  } else {
    // TODO: Fill this in ...
  }
  resume();
}

/*-----------------------------------------------------------------------*/

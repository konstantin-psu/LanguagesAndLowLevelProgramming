/*-------------------------------------------------------------------------
 * kernel.c:
 * Mark P Jones + YOUR NAME HERE, Portland State University *-----------------------------------------------------------------------*/
#include "winio.h"
#include "util.h"
#include "mimguser.h"
#include "context.h"
#include "proc.h"
#include "paging.h"
#include "memory.h"
#include "hardware.h"
#include "alloc.h"
#include "caps.h"

/*-------------------------------------------------------------------------
 * Two output windows for user level processes:
 */
static struct Window upperRight;
static struct Window lowerRight;

/*-------------------------------------------------------------------------
 * The main "kernel" code:
 */
void kernel() {
    // ----------------------------------------------------------------------
    // Configure initial screen layout:
    setVideo(KERNEL_SPACE+0xb8000);
    setAttr(0x2e);
    cls();
    printf(" kernel output (console):                      upperRight:");
    printf("\n\n\n\n\n\n\n\n\n\n\n\n");
    printf("                                               lowerRight:");
    printf("\n\n\n\n\n\n\n\n\n\n\n\n");
    printf(" Welcome to the LLP Capabilities Lab!");
    setAttr(7);
    setWindow(1, 23, 1, 45);   // kernel on left hand side
    cls();
    printf("Paging kernel has booted!\n");
    printf("kernel code is at 0x%x\n", kernel);

    // ----------------------------------------------------------------------
    // Configure the user program output windows:
    wsetWindow(&upperRight, 1, 11, 47, 32);
    wsetAttr(&upperRight, 1); // Blue output for upper window
    printf("upperRight is at %x\n", &upperRight);

    wsetWindow(&lowerRight,  13, 11, 47, 32);
    wsetAttr(&lowerRight, 4);   // Red output for lower window
    printf("lowerRight is at %x\n", &lowerRight);


    // ----------------------------------------------------------------------
    // Initialize kernel memory:
    struct BootData* bd = fromPhys(struct BootData*, 0x1000);
    unsigned* hdrs      = fromPhys(unsigned*, (unsigned)bd->headers);
    unsigned* mmap      = fromPhys(unsigned*, (unsigned)bd->mmap);

    initMemory(hdrs, mmap);

    // ----------------------------------------------------------------------
    // Allocate some pages and check that the addresses are
    // within the allocator region, and increasing from one
    // call to the next:
    printf("Allocating a page at: %x\n", allocPage());
    printf("Allocating a page at: %x\n", allocPage());
    printf("Allocating a page at: %x\n", allocPage());

    // ----------------------------------------------------------------------
    // Configure proc[0]:
    initProcess(proc+0, hdrs[7], hdrs[8], hdrs[9]);
    printf("cspace is at %x, cap is at %x\n",
            proc[0].cspace,
            proc[0].cspace->caps + 1);
    cspaceLoop(proc[0].cspace, 0);
    consoleCap(proc[0].cspace->caps + 1, 4);
    windowCap(proc[0].cspace->caps  + 2,
            &upperRight,
            /*CAN_cls|*/CAN_setAttr|CAN_putchar);
    donateUntyped(proc[0].cspace->caps + 3, 0);
    timeCap(proc[0].cspace->caps + 4, 3);
    showCspace(proc[0].cspace);

    // ----------------------------------------------------------------------
    // Configure proc[1]:
    initProcess(proc+1, hdrs[7], hdrs[8], hdrs[9]);
    cspaceLoop(proc[1].cspace, 0);
    timeCap(proc[1].cspace->caps + 4, 3);
    windowCap(proc[1].cspace->caps  + 2,
            &lowerRight,
            /*CAN_cls|*/CAN_setAttr|CAN_putchar);
    donateUntyped(proc[1].cspace->caps + 3, 0);
    showCspace(proc[1].cspace);

    // ----------------------------------------------------------------------
    // Prepare to start running user processes:
    current = proc;
    printf("current is at %x\n", (unsigned)(current));

    printf("starting timer!\n");
    startTimer();

    // ----------------------------------------------------------------------
    // Switch to proc[0]:
    setPdir(toPhys(current->pdir));
    switchToUser(&current->ctxt);

    // ----------------------------------------------------------------------
    printf("The kernel will now halt!\n");
    halt();
}

/*-----------------------------------------------------------------------*/

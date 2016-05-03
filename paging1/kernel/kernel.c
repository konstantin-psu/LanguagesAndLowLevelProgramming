/*-------------------------------------------------------------------------
 * kernel.c:
 * Mark P Jones + YOUR NAME HERE, Portland State University
 *-----------------------------------------------------------------------*/
#include "simpleio.h"
#include "mimguser.h"
#include "context.h"
#include "paging.h"
#include "memory.h"

/*-------------------------------------------------------------------------
 * Basic code for halting the processor and reporting a fatal error:
 */
extern void halt();

void fatal(char* msg) {
    printf("FATAL ERROR: %s\n", msg);
    halt();
}

/*-------------------------------------------------------------------------
 * Memory management: simple functionality for allocating pages of
 * memory for use in constructing page tables, etc.
 */
unsigned physStart;  // Set during initialization to start of memory pool
unsigned physEnd;    // Set during initialization to end of memory pool

unsigned* allocPage() {
    //printf("You have not implemented the allocPage() function (yet)!\n");

    unsigned size = 0;
    size  = physEnd - physStart;
    unsigned i = 0;
    unsigned r = physStart;
    struct Pdir * pdir = fromPhys(struct Pdir *, r);

    if (size < 0x1000) {
        fatal("-- allocPage -- : not enough space left");
    }

    printf("-- allocPage -- : starting search trough the available memory\n");
    physStart = pageNext(physStart);   // advance physStart
    size = 0x1000;
    printf("-- allocPage -- : allocating at 0x%x physstart at %x\n",pdir, physStart);
    for (i = 0; i < 1024; i++) {
        pdir->pde[i] = 0;
        //*(unsigned *)(r + i)  = 0x0;
    }

    // TODO: Allocate a page of data, starting at the
    // physical address that is specified by physStart,
    // but returning a pointer to the region in virtual
    // memory.  The implementation should:
    //   - Trigger a fatal error if there is not enough
    //     space left in [physStart .. physEnd)
    //   - Write zero in to every byte (or word, for a
    //     more efficient implementation) of the page
    //     before returning a pointer.
    //   - Update physStart so that it points to the
    //     next available page in memory after the
    //     allocation is complete ...
    //struct Pdir * pdir = fromPhys(struct Pdir *, r);

    return pdir; // TODO: Replace this placeholder with the proper value
}

void mapRegion(struct Pdir* aPdir, unsigned lo, unsigned hi) {
    printf("-- mapRegion -- : Entry\n");
    while (lo < hi) {
        mapPage(aPdir, lo, lo);
        lo = pageNext(lo);
    }
}

/*-------------------------------------------------------------------------
 * Context data structures: a place holder for when we get back to
 * context switching ...
 */
struct Context user;

/*-------------------------------------------------------------------------
 * The main "kernel" code:
 */
void kernel() {
    struct BootData* bd = (struct BootData*)0x1000;
    unsigned* hdrs      = bd->headers;
    unsigned* mmap      = bd->mmap;
    unsigned  i;
    unsigned int userHi, userLo;

    setAttr(0x2e);
    cls();
    setAttr(7);
    setWindow(1, 23, 1, 45);   // kernel on left hand side
    cls();
    printf(" -- kernel -- : Paging kernel has booted!\n");

    printf(" -- kernel -- : Headers:\n");
    for (i=0; i<hdrs[0]; i++) {
        printf(" -- kernel -- : header[%d]: [%x-%x], entry %x\n",
                i, hdrs[3*i+1], hdrs[3*i+2], hdrs[3*i+3]);
    }
    userLo = hdrs[7];
    userHi = hdrs[8];

    printf(" -- kernel -- : Memory map:\n");
    for (i=0; i<mmap[0]; i++) {
        printf(" -- kernel -- : mmap[%d]: [%x-%x]\n",
                i, mmap[2*i+1], mmap[2*i+2]);
    }

    printf("-- kernel -- :Strings:\n");
    printf("-- kernel -- : cmdline: %s [%x]\n", bd->cmdline, bd->cmdline);
    printf("-- kernel -- : imgline: %s [%x]\n", bd->imgline, bd->imgline);

    extern struct Pdir initdir[];
    printf("-- kernel -- : initial page directory is at 0x%x\n", initdir);
    showPdir(initdir);

    printf("-- kernel -- : kernel code is at 0x%x\n", kernel);

    // TODO: Scan the memory map to find the biggest region of
    // available pages subject to the constraints:
    //  - The start address (which should be stored in physStart)
    //    must have a zero page offset and must be >= KERNEL_LOAD,
    //    the address at which the kernel is loaded.
    //  - The end address (which should be stored in physEnd) must
    //    have an offset of 0xfff (i.e., corresponding to the last
    //    byte in a page); it must be greater than physStart; and
    //    it must be less than PHYSMAP.  (It cannot be equal to
    //    PHYSMAP because PHYSMAP has zero offset.)

    unsigned test = KERNEL_LOAD;
    unsigned phmap = PHYSMAP;
    printf("-- kernel -- : KERNEL_LOAD 0x%x\n", test);
    printf("-- kernel -- : PHYSMAP 0x%x\n", phmap);
    unsigned max = 0;
    unsigned prev = 0;
    // TODO: Remove the following halt() call once you are
    // ready to proceed!
    for (i=0; i<mmap[0]; i++) {
        if (mmap[2*i+1] >= KERNEL_LOAD && !(maskTo(mmap[2*i+2],12) ^ 0xfff)) {
            max = mmap[2*i+2] - mmap[2*i+1] > prev ? i  : max;
            prev = mmap[2*max+2] - mmap[2*max+1];
            printf("-- kernel -- : %d %x %x max %d diff %x\n",i, mmap[2*i+2], mmap[2*i+1], max, mmap[2*i+2] - mmap[2*i+1]);
        }
    }
    physStart = mmap[max*2+1];
    physEnd = mmap[max*2+2];
    printf("-- kernel -- : physStart so far %x, physEnd so far %x\n", physStart, physEnd);

    // TODO: Report a fatal error if there is no suitable region
    // of memory.

    if (!max) {
        printf("-- kernel -- : FATAL ERROR: no suitable region of memory found\n");
        halt();
    }

    // TODO: Scan the list of headers for loaded regions of memory
    // to look for conflicts with the [physStart..physEnd) region.
    // If you find a conflict, increase physStart to point to the
    // start of the first page after the conflicting region.
    unsigned nextConflictStart = 0xffffffff;
    unsigned gapSize = 0;
    unsigned gapLocationStart = 0;

    printf("\n\n");
    for (i=0; i<hdrs[0]; i++) {
        unsigned overlapEnd = hdrs[3*i +2];
        unsigned overlapStart = hdrs[3*i +1];
        if (overlapEnd >= physStart && overlapStart <= physEnd) {
            unsigned startPage = physStart;
            if (overlapStart > startPage) {
                if (overlapStart - startPage > gapSize) {
                    gapSize = overlapStart - startPage;
                    gapLocationStart = startPage;
                }
            }
            while (startPage < overlapEnd) 
                startPage = pageNext(startPage);
            physStart = startPage;
            printf("-- kernel -- : physStart %x, physEnd %x gapSize %d gapLocationStart %x \n",
                    physStart, physEnd, gapSize, gapLocationStart);
            printf("-- kernel -- : header[%d]: [%x-%x], entry %x\n",
                    i, hdrs[3*i+1], hdrs[3*i+2], hdrs[3*i+3]);
        }
    }

    if (gapSize > physEnd - physStart) {
        physStart = gapLocationStart;
        physEnd = physStart + gapSize;
    }


    // TODO: Report a fatal error if this process has resulting in
    // an empty region of physical memory.

    // Display the upper and lower bounds of the chosen memory
    // region, as well as the total number of bytes that it
    // contains.

    printf("-- kernel -- : Will allocate from region [%x-%x], %d bytes\n",
            physStart, physEnd, (1 + physEnd - physStart));

    // Now we will build a new page directory:
    struct Pdir* newpdir = allocPdir();

    // TODO: You might need to do something more to the newly
    // created page directory, but who knows what that might
    // be ... ?   :-)

    // printf("Show pagedir FINISHED\n");
    // printf("toPhysInitial %x, toPhysNew %x\n",toPhys(initdir),toPhys(newpdir));
    // printf("printf address %x phys %x\n", printf, toPhys(printf));
    //mapPage(newpdir, newpdir, toPhys(newpdir));
    mapPage(newpdir, 0xb8000, 0xb8000); // !!!!!!!!!!!!!!!

    showPdir(newpdir);

#define IM_FEELING_LUCKY
#ifdef IM_FEELING_LUCKY
    // halt();
    setPdir(toPhys(newpdir));
    //setPdir(toPhys(initdir));
    printf("-- kernel -- : I am feeling lucky\n");
#else
    printf("-- kernel -- : Switch to the new page directory when you're ready!\n");
#endif

    // TODO: reinstate the following code ... but we'll get to that
    // next time!
    // bd = fromPhys(struct BootData*, 0x1000);
    // hdrs      = bd->headers;
    // mmap      = bd->mmap;

    printf("-- kernel -- : user lo 0x%x user hi 0x%x\n", userLo, userHi);
    mapRegion(newpdir, userLo, userHi);
    // printf("user code is at 0x%x\n", hdrs[9]);
    // initContext(&user, hdrs[9], 0);
    // printf("user is at %x\n", (unsigned)(&user));
    // switchToUser(&user);
    showPdir(newpdir);

    printf("-- kernel -- : The kernel will now halt!\n");
    halt();
}

void csyscall() {  /* A trivial system call */
    putchar(user.regs.eax);
    switchToUser(&user);
}

/*-----------------------------------------------------------------------*/

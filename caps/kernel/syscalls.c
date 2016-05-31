/*-------------------------------------------------------------------------
 * syscalls.c: System call handlers
 * Mark P Jones + YOUR NAME HERE, Portland State University
 *-----------------------------------------------------------------------*/
#include "winio.h"
#include "util.h"
#include "context.h"
#include "proc.h"
#include "paging.h"
#include "memory.h"
#include "hardware.h"
#include "alloc.h"
#include "caps.h"


static unsigned ticks = 0;
static unsigned kernelStart = 0xc0000000;

void mapPageTable(struct Pdir* pdir, unsigned virt, unsigned phys) {
    // Mask out the least significant 12 bits of virt and phys.
    virt = alignTo(virt, PAGESIZE);
    // phys = alignTo(phys, PAGESIZE);

    // Make sure that the virtual address is in user space.
    if (virt>=KERNEL_SPACE) {
        fatal("virtual address is in kernel space");
    }

    // Find the relevant entry in the page directory
    unsigned dir = maskTo(virt >> SUPERSIZE, 10);
    unsigned pde = pdir->pde[dir];

    // Report a fatal error if there is already a
    // superpage mapped at that address (this shouldn't
    // be possible at this stage, but we're programming
    // defensively).
    if ((pde&0x81)==0x81) {
        fatal("Address is already mapped (as a superpage)");
    }

    // Which page table entry do we want?
        // struct Ptab* ptab = (struct Ptab *) phys;
    //struct Ptab* ptab;
    unsigned idx = maskTo(virt >> PAGESIZE, 10);
    if ((pde&1)==0) {  // page table not present; make a new one!
        // struct Ptab* ptab = (struct Ptab *) phys;
        // ptab = allocPtab();
        pdir->pde[dir] = phys | PERMS_USER_RW;
    } 
}

void mapCapPage(struct Pdir* pdir, unsigned virt, unsigned phys) {
  // Mask out the least significant 12 bits of virt and phys.
  virt = alignTo(virt, PAGESIZE);
  phys = alignTo(phys, PAGESIZE);

  // Make sure that the virtual address is in user space.
  if (virt>=KERNEL_SPACE) {
    fatal("virtual address is in kernel space");
  }

  // Find the relevant entry in the page directory
  unsigned dir = maskTo(virt >> SUPERSIZE, 10);
  unsigned pde = pdir->pde[dir];

  // Report a fatal error if there is already a
  // superpage mapped at that address (this shouldn't
  // be possible at this stage, but we're programming
  // defensively).
  if ((pde&0x81)==0x81) {
    fatal("Address is already mapped (as a superpage)");
  }

  // Which page table entry do we want?
  struct Ptab* ptab;
  unsigned idx = maskTo(virt >> PAGESIZE, 10);
  if ((pde&1)==0) {  // page table not present; make a new one!
    // If there is no page table (i.e., the PDE is empty),
    // then allocate a new page table and update the pdir
    // to point to it.   (use PERMS_USER_RW together with
    // the new page table's *physical* address for this.)
    fatal("Page Table is not Present");
  } else {
    ptab = fromPhys(struct Ptab*, alignTo(pde, PAGESIZE));
    // If there was an existing page table (i.e., the PDE
    // pointed to a page table), then report a fatal error
    // if the specific entry we want is already in use.
    if (ptab->pte[idx]&1) {
      fatal("Page is already mapped");
    }
  }

  // Add an entry in the page table structure to complete
  // the mapping of virt to phys.  (Use PERMS_USER_RW
  // again, this time combined with the value of the
  // phys parameter that was supplied as an input.)
  ptab->pte[idx] = phys | PERMS_USER_RW;
}

/*-------------------------------------------------------------------------
 * System call to dump current status data for the invoking thread in the
 * console window.
 */
void syscallDump() {
    struct Context* ctxt = &current->ctxt;
    showPdir(current->pdir);
    showCspace(current->cspace);
    showUntyped();
    switchToUser(ctxt);
}

/*-------------------------------------------------------------------------
 * System call to map a page at a specific address using the kernel to
 * allocate the necessary storage for the page and, possibly, a page table.
 */
// Determine if there is already a mapping for a page at a given address.
static unsigned isMapped(struct Pdir* pdir, unsigned virt) {
    virt = alignTo(virt, PAGESIZE);
    if (virt<KERNEL_SPACE) {
        unsigned dir = maskTo(virt >> SUPERSIZE, 10);
        unsigned pde = pdir->pde[dir];
        if ((pde&1)==0) {                 // directory entry not in use
            return 0;
        } else if ((pde&0x80)==0) {       // not mapped to a superpage
            unsigned     idx  = maskTo(virt >> PAGESIZE, 10);
            struct Ptab* ptab = fromPhys(struct Ptab*, alignTo(pde, PAGESIZE));
            return (ptab->pte[idx] & 1);    // return 0 if already mapped
        }
    }
    return 1;
}

static unsigned isTableMapped(struct Pdir* pdir, unsigned virt) {
    virt = alignTo(virt, PAGESIZE);
    if (virt<KERNEL_SPACE) {
        unsigned dir = maskTo(virt >> SUPERSIZE, 10);
        unsigned pde = pdir->pde[dir];
        if ((pde&1)==0) {                 // directory entry not in use
            return 0;
        } else {       // not mapped to a superpage
            return 1; 
        }
    }
    return 0;
}

void syscallKmapPage() {
    struct Context* ctxt = &current->ctxt;
    unsigned        addr = ctxt->regs.esi;
    unsigned*       page;
    if (!isMapped(current->pdir, addr) && (page=allocPage())) {
        //printf("Mapping to virtual address %x to page at %x\n", addr, page);
        mapPage(current->pdir, addr, toPhys(page));
        ctxt->regs.eax = 1;
    } else {
        printf("Could not map a page at %x\n", addr);
        ctxt->regs.eax = 0;
    }
    switchToUser(ctxt);
}

/*-------------------------------------------------------------------------
 * Console (kernel output window) system call.
 */
void syscallKputc() {
    struct Context*    ctxt = &current->ctxt;
    struct ConsoleCap* cap  = isConsoleCap(current->cspace->caps +
            cptr(ctxt->regs.ecx));
    if (cap) {
        setAttr(cap->attr);
        putchar(ctxt->regs.eax);
        setAttr(7);
        ctxt->regs.eax = (unsigned)current;
    } else {
        ctxt->regs.eax = 0;
    }
    switchToUser(ctxt);
}

/*-------------------------------------------------------------------------
 * Window output system calls.
 */
struct WindowCap* getWindowCap() {
    return isWindowCap(current->cspace->caps + cptr(current->ctxt.regs.ecx));
}

void syscallPutchar() {
    struct WindowCap* wcap = getWindowCap();
    if (wcap && (wcap->perms & CAN_putchar)) {
        wputchar(wcap->window, current->ctxt.regs.eax);
    }
    switchToUser(&current->ctxt);
}

void syscallCls() {
    struct WindowCap* wcap = getWindowCap();
    // DONE: complete implementation for this function:
    // do not allow the window to be cleared unless the
    // invoking process has specified a valid WindowCap
    // with CAN_cls permission.  The capability number
    // will be specified in the ecx register.
    if (wcap && (wcap->perms & CAN_cls)) {
        wcls(wcap->window);
    }
    switchToUser(&current->ctxt);
}

void syscallSetAttr() {
    struct WindowCap* wcap = getWindowCap();
    // DONE: complete implementation for this function
    // do not allow the window to be cleared unless the
    // invoking process has specified a valid WindowCap
    // with CAN_setAttr permission.  The capability number
    // will be specified in the ecx register, while the
    // value in eax determines the new attribute.
    if (wcap && (wcap->perms & CAN_setAttr)) {
        wcls(wcap->window);
    }
    switchToUser(&current->ctxt);
}

/*-------------------------------------------------------------------------
 * Lookup a capability that is specified by an index and a cptr pair.
 */
static struct Cap* getCap(unsigned slot) {
    printf("getCap(%x), index=%x, cptr=%x\n", slot, index(slot), cptr(slot));
    struct Cspace* cspace = isCspaceCap(current->cspace->caps + index(slot));
    printf("cspace = %x, ret=%x\n", cspace,
            (cspace ? (cspace->caps + cptr(slot)) : 0));
    return cspace ? (cspace->caps + cptr(slot)) : 0;
}

void syscallCapmove() {
    struct Context* ctxt = &current->ctxt;
    struct Cap*     src  = getCap(ctxt->regs.esi);
    struct Cap*     dst  = getCap(ctxt->regs.edi);
    unsigned        copy = ctxt->regs.eax;
    printf("Capmove: %x -> %x %s\n",
            ctxt->regs.esi,
            ctxt->regs.edi,
            (copy ? "copy" : "move"));
    printf("  src=%x, dst=%x\n", src, dst);
    if ((dst && src && isNullCap(dst) && !isNullCap(src)) &&
            (!copy || src->type!=UntypedCap)) { // Cannot copy an untyped cap
        printf("  Before:\n");
        showCspace(current->cspace);
        moveCap(src, dst, ctxt->regs.eax);
        printf("  After:\n");
        showCspace(current->cspace);
        ctxt->regs.eax = 1;
    } else {
        printf("  Invalid capmove\n");
        ctxt->regs.eax = 0;
    }
    switchToUser(ctxt);
}

void syscallCapclear() {
    struct Context* ctxt = &current->ctxt;
    // DONE: complete implementation for this function:
    // Input argument (in esi) provides an index in the
    // invoking process' cpsace to a CspaceCap as well
    // as a cptr (in the least significant byte) to a
    // particular slot in that cspace.  If the referenced
    // capability can be found and is not a NullCap, then
    // it will be set to NullCap and the value 1 will be
    // returned in eax.  If the operation is not permitted
    // then the return code will be zero.

    struct Cap*     src  = getCap(ctxt->regs.esi);
    if ((src && !isNullCap(src))) {
        showCspace(current->cspace);
        nullCap(src);
        showCspace(current->cspace);
        ctxt->regs.eax = 1;
    } else {
        ctxt->regs.eax = 0;
    }
    switchToUser(ctxt);
}

/*-------------------------------------------------------------------------
 * System calls for allocating objects from an untyped memory area.
 *
 * In each case:
 *   ecx = capability to untyped memory
 *   edi = destination slot for capability to new object
 *
 * These allocator calls are very similar (the only differences being in
 * the way that the new objects are initialized and the ability to allow
 * a variable (bits) size for untyped objects).  It might be nice to
 * combine them in a single system call that takes the object type as an
 * extra parameter.
 */
struct UntypedCap* getUntypedCap() {
    return isUntypedCap(current->cspace->caps + cptr(current->ctxt.regs.ecx));
}

// allocUntyped also uses: eax = log size of new untyped to allocate
void syscallAllocUntyped() {
    struct Context*    ctxt = &current->ctxt;
    struct UntypedCap* ucap = getUntypedCap();
    unsigned           bits = ctxt->regs.eax;
    retype(ucap, 4, bits,  ctxt->regs.edi, 1);
}

void syscallAllocCspace() {
    struct Context*    ctxt = &current->ctxt;
    struct UntypedCap* ucap = getUntypedCap();
    retype(ucap, 3, PAGESIZE,  ctxt->regs.edi, 1);
}


void syscallAllocPage() {
    struct Context*    ctxt = &current->ctxt;
    // DONE: Using the capability to untyped memory that is referenced
    // by ecx in the invoking process' cspace, allocate a new Page
    // object and place a capability to the new object in the slot of
    // the invoking process' cspace at the position specified by edi.
    // Return code in eax should be 1 for success, 0 for failure.

    struct UntypedCap* ucap = getUntypedCap();
    retype(ucap, 5, PAGESIZE,  ctxt->regs.edi, 1);
}


void syscallAllocPageTable() {
    struct Context*    ctxt = &current->ctxt;
    // DONE: Using the capability to untyped memory that is referenced
    // by ecx in the invoking process' cspace, allocate a new Page
    // object and place a capability to the new object in the slot of
    // the invoking process' cspace at the position specified by edi.
    // Return code in eax should be 1 for success, 0 for failure.
    struct UntypedCap* ucap = getUntypedCap();
    retype(ucap, 6, PAGESIZE,  ctxt->regs.edi, 1);
}



/*-------------------------------------------------------------------------
 * Address space management:
 */
struct PageCap* getPageCap() {
    return isPageCap(current->cspace->caps + cptr(current->ctxt.regs.ecx));
}

struct PageTableCap* getPageTableCap() {
    return isPageTableCap(current->cspace->caps + cptr(current->ctxt.regs.ecx));
}

 void mapPageTableFromCap(struct Ptab* ptab, unsigned virt, unsigned phys) {
  // Mask out the least significant 12 bits of virt and phys.
  virt = alignTo(virt, PAGESIZE);
  phys = alignTo(phys, PAGESIZE);

  // Make sure that the virtual address is in user space.
  if (virt>=KERNEL_SPACE) {
    fatal("virtual address is in kernel space");
  }

  unsigned idx = maskTo(virt >> PAGESIZE, 10);
  if (ptab->pte[idx]&1) {
    fatal("Page is already mapped");
  }

  // Add an entry in the page table structure to complete
  // the mapping of virt to phys.  (Use PERMS_USER_RW
  // again, this time combined with the value of the
  // phys parameter that was supplied as an input.)
  ptab->pte[idx] = phys | PERMS_USER_RW;
}
void syscallMapPage() {
    struct Context* ctxt = &current->ctxt;
    // TODO: This system call takes two parameters:
    //   ecx should identify a capability in the invoking process'
    //       cspace to a Page object:
    //   eax should specify an address in the current process'
    //       address space where that page should be mapped.
    // For this operation to succeed: the address should be within
    // the user space region of memory; there should already be a
    // page table (not a super page) in the appropriate slot of the
    // page directory; but there should not be a page mapped at the
    // specified address.  You may choose either to ignore the least
    // significant bits of the address (the page offset) or, if you
    // prefer, to decline the request for a mapping if those bits
    // are not zero.  A return code of 1 (success) or 0 (error)
    // should be produced as normal.

    struct PageCap* pcap = getPageCap();
    unsigned addr = ctxt->regs.eax;
    struct Cap*        cap  = getCap(ctxt->regs.edi);
    void*              obj;

    showPdir(current->pdir);
    //
    unsigned * page;
    if (isTableMapped(current->pdir, addr) && (pcap=getPageCap())) {
        // TODO FIXME
        mapCapPage(current->pdir, addr, toPhys(pcap->page));
        showPdir(current->pdir);
        ctxt->regs.eax = 1;
    } else {
        ctxt->regs.eax = 0;
    }
    switchToUser(ctxt);
}


void syscallMapPageTable() {
    struct Context* ctxt = &current->ctxt;
    // TODO: This system call takes two parameters:
    //   ecx should identify a capability in the invoking process'
    //       cspace to a PageTable object:
    //   eax should specify an address in the current process'
    //       address space where that pagetable should be mapped.
    // For this operation to succeed: the address should be within
    // the user space region of memory and the corresponding entry
    // in the page directory should not be present.  You may choose
    // either to ignore the least significant 22 bits of the address
    // (the superpage offset) or, if you prefer, to decline the
    // request for a mapping if those bits are not zero.  A return
    // code of 1 (success) or 0 (error) should be produced as normal.
    struct PageTableCap* pcap = getPageTableCap();
    unsigned addr = ctxt->regs.eax;

    if (addr >= KERNEL_SPACE) {
        ctxt->regs.eax = 0;
        switchToUser(ctxt);
    }

    struct Cap*        cap  = getCap(ctxt->regs.edi);
    void*              obj;
    showPdir(current->pdir);
    // printf("**syscall.c** --shscallAllocPage-- : current cpaces");
    //

    unsigned * page;
    if (!isTableMapped(current->pdir, addr) && (pcap=getPageTableCap())) {
        mapPageTable(current->pdir, addr, toPhys(((unsigned*)pcap->ptab)));
        ctxt->regs.eax = 1;
    } else {
        ctxt->regs.eax = 0;
    }


    switchToUser(ctxt);
}

// static inline struct UntypedCap* isUntypedCap(struct Cap* cap) {
//   return (cap->type==UntypedCap) ? (struct UntypedCap*)cap : 0;
// }

// struct UntypedCap* getUntypedCap() {
//  return isPageUntypedCap(current->cspace->caps + cptr(current->ctxt.regs.ecx));
// }

void syscallRemaining() {
    struct Context* ctxt = &current->ctxt;
    struct UntypedCap* ucap = getUntypedCap();
    if (ucap) {
        ctxt->regs.eax = (1<<ucap->bits) - ucap->next;
    } else {
        ctxt->regs.eax = 0;
    }


    switchToUser(ctxt);
}

static inline struct TimeCap* isTimeCap(struct Cap* cap) {
    return (cap->type==TimeCap) ? (struct TimeCap*)cap : 0;
}

struct TimeCap* getTimeCap() {
    return isTimeCap(current->cspace->caps + cptr(current->ctxt.regs.ecx));
}

void syscallGetTotalTicks() {
    struct Context* ctxt = &current->ctxt;
    struct TimeCap* tcap = getTimeCap();
    if (tcap) {
        ctxt->regs.eax = (ticks >> tcap->resolution) << tcap->resolution;
    } else {
        ctxt->regs.eax = 0;
    }


    switchToUser(ctxt);
}

// void syscallPutchar() {
//     struct WindowCap* wcap = getWindowCap();
//     if (wcap && (wcap->perms & CAN_putchar)) {
//         wputchar(wcap->window, current->ctxt.regs.eax);
//     }
//     switchToUser(&current->ctxt);
// }
//
//   printf("  Page directory at %x\n", pdir);
//   for (unsigned i=0; i<1024; i++) {
//     if (pdir->pde[i]&1) {
//       if (pdir->pde[i]&0x80) {
//         printf("    %x: [%x-%x] => [%x-%x], superpage\n",
//                i, (i<<SUPERSIZE), ((i+1)<<SUPERSIZE)-1,
//                alignTo(pdir->pde[i], SUPERSIZE),
//                alignTo(pdir->pde[i], SUPERSIZE) + 0x3fffff);
//       } else {
//         struct Ptab* ptab = fromPhys(struct Ptab*,
//                                      alignTo(pdir->pde[i], PAGESIZE));
//         unsigned base = (i<<SUPERSIZE);
//         printf("    [%x-%x] => page table at %x (physical %x):\n",
//                base, base + (1<<SUPERSIZE)-1,
//                ptab, alignTo(pdir->pde[i], PAGESIZE));
//         for (unsigned j=0; j<1024; j++) {
//           if (ptab->pte[j] & 1) {
//             printf("      %x: [%x-%x] => [%x-%x] page\n",
//                    j, base+(j<<PAGESIZE), base + ((j+1)<<PAGESIZE) - 1,
//                    alignTo(ptab->pte[j], PAGESIZE),
//                    alignTo(ptab->pte[j], PAGESIZE) + 0xfff);
//           }
//         }
//       }
//     }
//   }

unsigned pageExists(struct Pdir * pdir, unsigned address) {
    // return End Address of the last page in contiguous memory space
    // otherwise return 0

    unsigned endAddress = 0;
    unsigned start = pageStart(address);
    while (1) {
        if (start >= kernelStart) {
            break;
        }
        unsigned dir = maskTo(start >> SUPERSIZE, 10);
        unsigned ptdir = maskTo(start >> PAGESIZE, 10);
        unsigned pden = pdir->pde[dir];
        unsigned pten = 0;

        if ((pden & 0x1) == 0) { // Test if page directory exists
            break;
        } else {
            struct Ptab * newTable = 0x0;
            newTable = fromPhys(struct Ptab *, alignTo(pdir->pde[dir],PAGESIZE));
            pten = newTable->pte[ptdir];
            if (!( pten && 0x1)) {
                break;
            }
            endAddress = pageEnd(start);
            start = pageNext(start);

        }
    }

    return endAddress;
}

void syscallPrintString() {
    // Before printing a region of memory need to verify that it doesn't violate
    // bounds of the available to user process memory.
    //
    //
    // 1. Find page bounds of the address to be printed.
    // 2. Scan memory starting at the print address, finishing 0
    //  2.1 Address should no exceed page bounds
    //  2.2 Address should belong only to the user process memory
    // 3. Verify that string is null terminated, if not add null
    // 4. Print 

    struct Context* ctxt = &current->ctxt;
    struct WindowCap* wcap = getWindowCap();
    char * printAddress = (char *)ctxt->regs.edi;
    unsigned pageBeginsAt = pageStart((unsigned) printAddress);
    unsigned pageEndsAt   = pageEnd  ((unsigned) printAddress);
    unsigned lastAddress = 0;

    if (wcap) {
        lastAddress = pageExists(current->pdir, (unsigned) printAddress);
        if (lastAddress) {
            // now need to scan for null
            char * temp = printAddress;
            while (*temp) {
                temp += 1;
                if (temp >= lastAddress) {
                    temp -=1;
                    *temp = 0; // not really safe, what if something important is overwritten?
                    break;
                }
            }
            ctxt->regs.eax = 1;
            wputs(wcap->window, printAddress);
        } else {
            ctxt->regs.eax = 0;
        }
    } else {
        ctxt->regs.eax = 0;
    }


    switchToUser(ctxt);
}

/*-------------------------------------------------------------------------
 * Timer interrupt handler:
 */
static void tick() {
    static char*    spin = "-\\|/";  // characters for a rotatating spinner
    static unsigned pos  = 0;        // current position within spinner
    ticks++;
    if ((ticks&15)==0) {
        // Calculate the memory location where the character in the top
        // right corner of the video RAM is stored (skipping the first
        // 79 columns, each of which takes two bytes).
        char *vid = (char*)(KERNEL_SPACE + 0xb8000 + (2*79));
        // Update the spinner character:
        *vid = spin[(pos++)&3];
        // Perform a context switch:
        reschedule();
    }
}

void timerInterrupt() {
    maskAckIRQ(TIMERIRQ);           // Mask and acknowledge timer interrupt
    enableIRQ(TIMERIRQ);            // TODO: can this be optimized?
    tick();
    switchToUser(&current->ctxt);
}

/*-----------------------------------------------------------------------*/

void retype(struct UntypedCap * ucap, unsigned type, unsigned bits, unsigned slot, unsigned num) {
    struct Context*    ctxt = &current->ctxt;
    struct Cspace* cspace = isCspaceCap(current->cspace->caps + index(slot));
    struct Cap* cap =  cspace ? (cspace->caps + cptr(slot)) : 0;
    void*  obj;
    printf("allocUntyped: bits %d from ucap=%x, slot=%x\n", bits, ucap, cap);
    struct PageCap*  pcap;
    struct PageTableCap*  ptcap;
    unsigned error = 0;
    if (ucap && cap  && isNullCap(cap)) // valid untyped capability && empty destination slot
    {
        for (int i = 0; i< num; i++) {
            if (!isNullCap(cspace->caps + cptr(slot + i))) {
                error++;    
                break;
            }
        }
        if (type == 4) {
            if (validUntypedSize(bits))
                error++;
        } 
        if (!error) {
            for (int i = 0; i< num; i++) {
                if (obj=alloc(ucap, 4)) { // object allocation succeeds
                    switch (type) {
                        case 3: // Cspace
                            cspaceCap(cap, (struct Cspace*)obj);
                            break;
                        case 4: // Utyped Capability
                            untypedCap(cap, obj, bits);
                            break;
                        case 5:  // Page Capability
                            pcap = (struct PageCap*)obj;
                            unsigned * page = (unsigned *)alloc(ucap,2);
                            pcap->page = page;
                            pageCap(cap, pcap);
                            break;
                        case 6:  // PageTable Capability
                            ptcap = (struct PageTableCap*)obj;
                            struct Ptab* ptabe = (struct Ptab *)alloc(ucap,bits);
                            ptcap->ptab = ptabe;
                            pageTableCap(cap, ptcap);
                            break;
                        default:
                            error++;
                    }
                    ctxt->regs.eax = 1;
                } else {
                    error++;
                }
            }
        }
    } else {
        error++;
    }

    if (error) 
    {
        ctxt->regs.eax = 0;
    }
    switchToUser(ctxt);
}

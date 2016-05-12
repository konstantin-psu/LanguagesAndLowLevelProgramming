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

#define DEBUG 1

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
    if (DEBUG)
        printf("--syscallCapclear-- : clearing Cap clear\n");
    if (DEBUG)
        printf("--syscallCapclear-- : reset: %x\n", ctxt->regs.esi);
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
    if (DEBUG)
        printf("--syscallCapclear-- :   src=%x\n", src);
    if ((src && !isNullCap(src))) {
        if (DEBUG)
            printf("--syscallCapclear-- : Capability is good to be reset\n");
        if (DEBUG)
            printf("--syscallCapclear-- : before reset\n");
        showCspace(current->cspace);
        nullCap(src);
        if (DEBUG)
            printf("--syscallCapclear-- : after reset\n");
        showCspace(current->cspace);
        ctxt->regs.eax = 1;
    } else {
        if (DEBUG)
            printf("--syscallCapclear-- : invalid cap or already NullCap\n");
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
    struct Cap*        cap  = getCap(ctxt->regs.edi);
    unsigned           bits = ctxt->regs.eax;
    void*              obj;
    printf("allocUntyped: bits %d from ucap=%x, slot=%x\n", bits, ucap, cap);
    if (ucap &&                     // valid untyped capability
            cap  && isNullCap(cap) &&   // empty destination slot
            validUntypedSize(bits) &&   // bit size in legal range
            (obj=alloc(ucap, bits))) {  // object allocation succeeds
        untypedCap(cap, obj, bits);
        ctxt->regs.eax = 1;
    } else {
        ctxt->regs.eax = 0;
    }
    switchToUser(ctxt);
}

void syscallAllocCspace() {
    struct Context*    ctxt = &current->ctxt;
    struct UntypedCap* ucap = getUntypedCap();
    struct Cap*        cap  = getCap(ctxt->regs.edi);
    void*              obj;
    printf("allocCspace: from ucap=%x, slot=%x\n", ucap, cap);
    if (ucap &&                        // valid untyped capability
            cap  && isNullCap(cap) &&      // empty destination slot
            (obj=alloc(ucap, PAGESIZE))) { // object allocation succeeds
        cspaceCap(cap, (struct Cspace*)obj);
        ctxt->regs.eax = 1;
    } else {
        ctxt->regs.eax = 0;
    }
    switchToUser(ctxt);
}

void syscallAllocPage() {
    struct Context*    ctxt = &current->ctxt;
    // DONE: Using the capability to untyped memory that is referenced
    // by ecx in the invoking process' cspace, allocate a new Page
    // object and place a capability to the new object in the slot of
    // the invoking process' cspace at the position specified by edi.
    // Return code in eax should be 1 for success, 0 for failure.

    printf("**syscall.c** --shscallAllocPage-- : Allocating new page\n");
    struct UntypedCap* ucap = getUntypedCap();
    struct Cap*        cap  = getCap(ctxt->regs.edi);
    void*              obj;
    printf("**syscall.c** --shscallAllocPage-- : from ucap=%x, slot=%x\n", ucap, cap);
    printf("**syscall.c** --shscallAllocPage-- : current cpaces");
    showCspace(current->cspace);
    if (ucap &&                        // valid untyped capability
            cap  && isNullCap(cap) &&      // empty destination slot
            (obj=alloc(ucap, PAGESIZE))) { // object allocation succeeds
        pageCap(cap, (struct PageCap*)obj);
        printf("**syscall.c** --shscallAllocPage-- : page allocated");
        printf("**syscall.c** --shscallAllocPage-- : cpaces agter page allocated");
        showCspace(current->cspace);
        ctxt->regs.eax = 1;
    } else {
        ctxt->regs.eax = 0;
    }

    switchToUser(ctxt);
}

void syscallAllocPageTable() {
    printf("**syscall.c** --syscallAllocPageTable-- : Allocating new page table\n");
    struct Context*    ctxt = &current->ctxt;
    // DONE: Using the capability to untyped memory that is referenced
    // by ecx in the invoking process' cspace, allocate a new Page
    // object and place a capability to the new object in the slot of
    // the invoking process' cspace at the position specified by edi.
    // Return code in eax should be 1 for success, 0 for failure.
    struct UntypedCap* ucap = getUntypedCap();
    struct Cap*        cap  = getCap(ctxt->regs.edi);
    void*              obj;
    // printf("**syscall.c** --shscallAllocPage-- : from ucap=%x, slot=%x\n", ucap, cap);
    // printf("**syscall.c** --shscallAllocPage-- : current cpaces");
    // showCspace(current->cspace);
    if (ucap &&                        // valid untyped capability
            cap  && isNullCap(cap) &&      // empty destination slot
            (obj=alloc(ucap, PAGESIZE))) { // object allocation succeeds
        pageTableCap(cap, (struct PageTableCap*)obj);
        // printf("**syscall.c** --shscallAllocPage-- : page allocated");
        // printf("**syscall.c** --shscallAllocPage-- : cpaces agter page allocated");
        //showCspace(current->cspace);
        ctxt->regs.eax = 1;
    } else {
        ctxt->regs.eax = 0;
    }
    switchToUser(ctxt);
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

void syscallMapPage() {
    printf("**syscall.c** --syscallMapPage-- : begin\n");
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

    // struct PageCap* pcap = getPageCap();
    // unsigned addr = ctxt->regs.eax;
    // struct Cap*        cap  = getCap(ctxt->regs.edi);
    // void*              obj;
    // printf("**syscall.c** --syscallMapPage-- : pcap=%x, slot=%x, addr 0x%x\n", pcap, cap, addr);
    // printf("**syscall.c** --syscallMapPage-- : current pdir");
    // showPdir(current->pdir);
    // // printf("**syscall.c** --shscallAllocPage-- : current cpaces");
    // // showCspace(current->cspace);
    // if (pcap &&                        // valid untyped capability
    //         cap  && isNullCap(cap) &&      // empty destination slot
    //         (obj=alloc(pcap, PAGESIZE))) { // object allocation succeeds
    //     pageTableCap(cap, (struct PageTableCap*)obj);
    //     // printf("**syscall.c** --shscallAllocPage-- : page allocated");
    //     // printf("**syscall.c** --shscallAllocPage-- : cpaces agter page allocated");
    //     //showCspace(current->cspace);
    //     printf("**syscall.c** --syscallMapPage-- : SUCCESS\n");
    //     ctxt->regs.eax = 1;
    // } else {
    //     printf("**syscall.c** --syscallMapPage-- : FAIL\n");
    //     ctxt->regs.eax = 0;
    // }
    switchToUser(ctxt);
}

void syscallMapPageTable() {
    printf("**syscall.c** --syscallMapPageTable-- : begin\n");
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
    struct Cap*        cap  = getCap(ctxt->regs.edi);
    void*              obj;
    printf("**syscall.c** --syscallMapPageTable-- : pcap=%x, slot=%x, addr 0x%x\n", pcap, cap, addr);
    printf("**syscall.c** --syscallMapPageTable-- : current pdir");
    showPdir(current->pdir);
    // printf("**syscall.c** --shscallAllocPage-- : current cpaces");
    // showCspace(current->cspace);
    if (pcap &&                        // valid untyped capability
            cap  && isNullCap(cap) &&      // empty destination slot
            (obj=alloc(pcap, PAGESIZE))) { // object allocation succeeds
        pageTableCap(cap, (struct PageTableCap*)obj);
        // printf("**syscall.c** --shscallAllocPage-- : page allocated");
        // printf("**syscall.c** --shscallAllocPage-- : cpaces agter page allocated");
        //showCspace(current->cspace);
        printf("**syscall.c** --syscallMapPageTable-- : SUCCESS\n");
        ctxt->regs.eax = 1;
    } else {
        printf("**syscall.c** --syscallMapPageTable-- : FAIL\n");
        ctxt->regs.eax = 0;
    }
    switchToUser(ctxt);
}

/*-------------------------------------------------------------------------
 * Timer interrupt handler:
 */
static void tick() {
    static unsigned ticks = 0;
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

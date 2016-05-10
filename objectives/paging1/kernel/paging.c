/*-------------------------------------------------------------------------
 * paging.c:
 * Mark P Jones, Portland State University
 *-----------------------------------------------------------------------*/
#include "winio.h"		// For printf (debugging output)
#include "paging.h"
#include "memory.h"

#define DEBUG 0
extern unsigned* allocPage();
extern void      fatal(char* msg);

/*-------------------------------------------------------------------------
 * Allocation of paging structures:
 */
struct Pdir* allocPdir() {
    // Allocate a fresh page directory:
    struct Pdir* pdir = (struct Pdir*)allocPage();
    unsigned dir = pdir;
    if (DEBUG)
        printf("-- paging allocPdir -- : physical address of the directory %x\n",dir);
    unsigned superPSize = 0;
    unsigned fl1 = 0;
    unsigned i = 0;
    superPSize = (PHYSMAP >> SUPERSIZE);
    unsigned * dp;
    //unsigned * dp1;
    if (DEBUG)
        printf("-- paging allocPdir -- : PHYSMAP %x SUPERSIZE %x superPSize %x\n",PHYSMAP, SUPERSIZE, superPSize);

    fl1 = (PERMS_KERNELSPACE);
    for (i = 0; i< superPSize; i++) {
        if (DEBUG)
            printf("-- paging allocPdir -- : i %d fl1 %x\n",i, fl1);
        //dp1 =  ((unsigned *)dir) +i;
        //*dp1 = fl1;
        dp = ((unsigned *)dir) + (KERNEL_SPACE >> SUPERSIZE) + i;
        *dp = fl1;
        // dir += 1;
        fl1 += 4 << 20;
    }


    // TODO: Add superpage mappings to pdir for the first PHYSMAP
    // bytes of physical memory.  You should use a bitwise or
    // operation to ensure that the PERMS_KERNELSPACE bits are set
    // in every PDE that you create.

    return pdir;
}

struct Ptab* allocPtab() {
    return (struct Ptab*)allocPage();
}

/*-------------------------------------------------------------------------
 * Create a mapping in the specified page directory that will map the
 * virtual address (page) specified by virt to the physical address
 * (page) specified by phys.  Any nonzero offset in the least
 * significant 12 bits of either virt or phys will be ignored.
 * 
 */
void mapPage(struct Pdir* pdir, unsigned virt, unsigned phys) {
    // Mask out the least significant 12 bits of virt and phys.
    virt = alignTo(virt, PAGESIZE);
    phys = alignTo(phys, PAGESIZE);

    unsigned pde;
    //unsigned entry;
    // Make sure that the virtual address is in user space.
    if (virt>=KERNEL_SPACE) {
        // printf("KERNEL_SPACE %x virtual address %x\n", KERNEL_SPACE, virt);
        fatal("--paging mapPage -- : virtual address is in kernel space");
    }
    if (DEBUG)
        printf("--paging mapPage -- : KERNEL_SPACE %x virtual address %x phys %x\n", KERNEL_SPACE, virt,phys);

    // TODO: Find the relevant entry in the page directory

    // TODO: report a fatal error if there is already a
    //       superpage mapped at that address (this shouldn't
    //       be possible at this stage, but we're programming
    //       defensively).


    unsigned dir = maskTo(virt >> SUPERSIZE, 10);
    unsigned ptdir = maskTo(virt >> PAGESIZE, 10);
    unsigned pden = pdir->pde[dir];
    if (DEBUG)
        printf("--paging mapPage -- : pden 0x%x\n",pden);
    struct Ptab * newTable = 0x0;
    if (DEBUG)
        printf("--paging mapPage -- : Current page directory entry 0x%x!!\n", pde);
    if ((pden & 0x81) == 0x81)
        fatal("--paging mapPage -- : Address is already mapped (as superpage)");


    if ((pden & 0x1) == 0) {
        if (DEBUG)
            printf("--paging mapPage -- : Creating new table\n");
        newTable = (struct Ptab *) allocPage();
    } else {
        newTable = fromPhys(struct Ptab *, alignTo(pdir->pde[dir],PAGESIZE));
        if ((newTable->pte[ptdir] && 0x1)) {
            fatal("--paging mapPage -- : Page is already mapped");
        }
    }
    if (DEBUG)
        printf("--paging mapPage -- : New table is at 0x%x!!\n", newTable);
    pdir->pde[dir] = toPhys(newTable) | PERMS_USER_RW;
    if (DEBUG)
        printf("--paging mapPage -- : Current page table entry 0x%x!!\n", pdir->pde[dir]);

    //newTable = (struct Ptab*)toPhys(newTable);
    newTable->pte[ptdir] = phys | PERMS_USER_RW;
    if (DEBUG)
        printf("--paging mapPage -- : New entry index %d\n", ptdir);


    // fromPhys(struct Ptab*, alignTo(pde, PAGESIZE));
    // TODO: If there is no page table (i.e., the PDE is empty),
    //       then allocate a new page table and update the pdir
    //       to point to it.   (use PERMS_USER_RW together with
    //       the new page table's *physical* address for this.)


    // TODO: If there was an existing page table (i.e., the PDE
    //       pointed to a page table), then report a fatal error
    //       if the specific entry we want is already in use.

    // TODO: Add an entry in the page table structure to complete
    //       the mapping of virt to phys.  (Use PERMS_USER_RW
    //       again, this time combined with the value of the
    //       phys parameter that was supplied as an input.)
}

/*-------------------------------------------------------------------------
 * Print a description of a page directory (for debugging purposes).
 */
void showPdir(struct Pdir* pdir) {
    printf("-- paging showPdir -- : Page directory at %x\n", pdir);
    for (unsigned i=0; i<1024; i++) {
        if (pdir->pde[i]&1) {
            if (pdir->pde[i]&0x80) {
                printf("    %x: [%x-%x] => [%x-%x], superpage\n",
                        i, (i<<SUPERSIZE), ((i+1)<<SUPERSIZE)-1,
                        alignTo(pdir->pde[i], SUPERSIZE),
                        alignTo(pdir->pde[i], SUPERSIZE) + 0x3fffff);
            } else {
                struct Ptab* ptab = fromPhys(struct Ptab*,
                        alignTo(pdir->pde[i], PAGESIZE));
                unsigned base = (i<<SUPERSIZE);
                printf("    [%x-%x] => page table at %x (physical %x):\n",
                        base, base + (1<<SUPERSIZE)-1,
                        ptab, alignTo(pdir->pde[i], PAGESIZE));
                for (unsigned j=0; j<1024; j++) {
                    if (ptab->pte[j] & 1) {
                        printf("      %x: [%x-%x] => [%x-%x] page\n",
                                j, base+(j<<PAGESIZE), base + ((j+1)<<PAGESIZE) - 1,
                                alignTo(ptab->pte[j], PAGESIZE),
                                alignTo(ptab->pte[j], PAGESIZE) + 0xfff);
                    }
                }
            }
        }
    }
}

/*-----------------------------------------------------------------------*/

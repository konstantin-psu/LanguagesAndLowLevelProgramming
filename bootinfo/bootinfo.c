/* ------------------------------------------------------------------------
 * bootinfo.c:  display basic boot information from GRUB
 *
 * Mark P. Jones, April 2016
 */

#include "simpleio.h"

/* ------------------------------------------------------------------------
 * Multiboot data structures:
 */
extern struct MultibootInfo* mbi;
extern unsigned              mbi_magic;
#define MBI_MAGIC 0x2BADB002

# define MBI_MEM_VALID            (1 << 0)
# define MBI_BOOT_DEVICE_VALID    (1 << 1)
# define MBI_CMD_VALID            (1 << 2)
# define MBI_MODS_VALID           (1 << 3)
# define MBI_MMAP_VALID           (1 << 6)

struct MultibootDrives {
   unsigned size;
   unsigned char  drive_number;
   unsigned char  drive_mode;
   unsigned short drive_cylinders;
   unsigned char  drive_heads;
   unsigned char  drive_sectors;
   short * drive_prots;
};

struct MultibootInfo {
  unsigned                  flags;
  unsigned                  memLower;
  unsigned                  memUpper;
  unsigned                  bootDevice;
  char*                     cmdline;
  unsigned                  modsCount;
  struct MultibootModule*   modsAddr;
  unsigned                  syms[4];
  unsigned                  mmapLength;
  struct MultibootMMap*     mmap;
  unsigned                  drivesLength;
  struct MultibootDrives*   drives;
  unsigned                  configTable;
  unsigned                  bootLoaderName;
  struct MultibootApmTable* apmTable;
  unsigned                  vbeControlInfo;
  unsigned                  vbeModeInfo;
  unsigned short            vbeMode;
  unsigned short            vbeInterfaceSeg;
  unsigned short            vbeInterfaceOff;
  unsigned short            vbeInterfaceLen;
};

struct MultibootModule {
  unsigned modStart;
  unsigned modEnd;
  char*    modString;
  unsigned reserved;
};

struct MultibootSymb {
  unsigned num;
  unsigned size;
  unsigned addr;
  unsigned shndx;
};

struct MultibootMMap {
  unsigned size;
  unsigned baseLo;
  unsigned baseHi;
  unsigned lenLo;
  unsigned lenHi;
  unsigned type;
};


struct MultibootApmTable {
   unsigned short version;
   unsigned short cseg;
   unsigned       offset;
   unsigned short cseg_16;
   unsigned short dseg;
   unsigned short flags;
   unsigned short cseg_len;
   unsigned short cseg_16_len;
   unsigned short dseg_len;
};

int print_flag(char * message, int flag_number) {
  if (((mbi->flags) & (1 << flag_number)) >> flag_number) {
    printf("%s : true\n", message, ((mbi->flags) & (1 << flag_number)) >> flag_number);
    return 1;
  } else {
    printf("%s : false\n", message, ((mbi->flags) & (1 << flag_number)) >> flag_number);
    return 0;
  }
  return 0;
}
//struct MultibootInfo {
//  unsigned                  flags;
//  unsigned                  memLower;
//  unsigned                  memUpper;
//  unsigned                  bootDevice;
//  char*                     cmdline;
//  unsigned                  modsCount;
//  struct MultibootModule*   modsAddr;
//  unsigned                  syms[4];
//  unsigned                  mmapLength;
//  unsigned                  mmapAddr;
//  unsigned                  drivesLength;
//  struct MultibootDrives*   drivesAddr;
//  unsigned                  configTable;
//  unsigned                  bootLoaderName;
//  struct MultibootApmTable* apmTable;
//  unsigned                  vbeControlInfo;
//  unsigned                  vbeModeInfo;
//  unsigned short            vbeMode;
//  unsigned short            vbeInterfaceSeg;
//  unsigned short            vbeInterfaceOff;
//  unsigned short            vbeInterfaceLen;
//};
/*-------------------------------------------------------------------------
 * Main program:
 */
void bootinfo() {
  int i = 0;
  cls();
  puts("bootinfo is running!\n");
  printf("mbi_magic : 0x%x\n", mbi_magic);
  printf("mbi       : 0x%x\n", mbi);
  printf("->flags   : 0x%x\n", mbi->flags);
  if (print_flag("0.   MEM_VALID",0)) {
    printf("    ->memLower: 0x%x\n", mbi->memLower);
    printf("    ->memUpper: 0x%x\n", mbi->memUpper);
  }
  if (print_flag("1.   BOOT_DEVICE_VALID"              , 1)) {
    printf("    ->bootdevice: 0x%x\n", mbi->bootDevice);
  }
  if (print_flag("2.   CMD_VALID"                      , 2)) {
    printf("    ->cmdline: 0x%x\n", mbi->cmdline);
  }
  if (print_flag("3.   MODS_VALID"                     , 3)) {
    printf("    ->modsCount: %d\n", mbi->modsCount);

  }
  if (print_flag("4.   SYMBOL_TABLE_VALID"             , 4)) {
    printf("    ->tabsize: %d\n", mbi->syms[0]);
    printf("    ->strsize: %d\n", mbi->syms[1]);
    printf("    ->addr:    %x\n", mbi->syms[2]);
  }
  if (print_flag("5.   ELF_HEADER_SECTION_VALID"       , 5)) {
    printf("    ->num: %d\n", mbi->syms[0]);
    printf("    ->size: %d\n", mbi->syms[1]);
    printf("    ->addr:    %x\n", mbi->syms[2]);
    printf("    ->shndx:    %x\n", mbi->syms[3]);
  }
  if (print_flag("6.   MBI_MMAP_VALID"                 , 6)) {
    printf("    ->mmap_legth: %d\n",   mbi->mmapLength);
    printf("    ->mmapSize:   %d\n", mbi->mmap->size);
    for (i = 0; i < mbi->mmap->size; i++) {
        printf("    ->MMAP ENTRY   :   %d\n", i);
        printf("        ->baseLo       :   0x%x\n", mbi->mmap->baseLo);
        printf("        ->baseHi       :   0x%x\n", mbi->mmap->baseHi);
        printf("        ->lenLo        :   0x%x\n", mbi->mmap->lenLo);
        printf("        ->lenHi        :   0x%x\n", mbi->mmap->lenHi);
        printf("        ->type         :   %d\n",   mbi->mmap->type);
    }
  }
  if (print_flag("7.   MBI_DRIVES_VALID"               , 7)) {
    printf("    ->drives_legth: %d\n", mbi->drivesLength);
    printf("    ->drivesSize:   %d\n", mbi->drives->size);
    for (i = 0; i < mbi->drives->size; i++) {
        printf("    ->DRIVE ENTRY   :   %d\n", i);
        printf("        ->driveNumber       :   %u\n", mbi->drives->drive_number);
        printf("        ->driveMode         :   %u\n", mbi->drives->drive_mode);
        printf("        ->driveCylinders    :   %hu\n",mbi->drives->drive_cylinders);
        printf("        ->driveHeads        :   %u\n", mbi->drives->drive_heads);
        printf("        ->driveSectors      :   %u\n",   mbi->drives->drive_sectors);
    }

  }
//struct MultibootApmTable {
//   unsigned short version;
//   unsigned short cseg;
//   unsigned       offset;
//   unsigned short cseg_16;
//   unsigned short dseg;
//   unsigned short flags;
//   unsigned short cseg_len;
//   unsigned short cseg_16_len;
//   unsigned short dseg_len;
  if (print_flag("8.   MBI_CONFIG_TABLE_VALID"         , 8)) {

  }
  if (print_flag("9.   MBI_BOOT_LOADER_NAME_FLAG_VALID", 9)) {
        printf("        ->bootLoaderName      :   %s\n",   mbi->bootLoaderName);

  }
  if (print_flag("10.  MBI_APM_TABLE_VALID"            , 10)) {
     printf("        ->version       :   %u\n", mbi->apmTable->version);
     printf("        ->cseg          :   %u\n", mbi->apmTable->cseg);
     printf("        ->offset        :   %d\n", mbi->apmTable->offset);
     printf("        ->cseg_16       :   %u\n", mbi->apmTable->cseg_16);
     printf("        ->dseg          :   %u\n", mbi->apmTable->dseg);
     printf("        ->flags         :   %u\n", mbi->apmTable->flags);
     printf("        ->cseg_len      :   %u\n", mbi->apmTable->cseg_len);
     printf("        ->cseg_16_len   :   %u\n", mbi->apmTable->cseg_16_len);
     printf("        ->dseg_len      :   %u\n", mbi->apmTable->dseg_len);
  }
  if (print_flag("11.  MBI_GRAPHICS_TABLE_VALID"       , 11)) {
     printf("        ->vbeControlInfo    :   %d\n", mbi->vbeControlInfo);
     printf("        ->vbeModeInfo       :   %d\n", mbi->vbeModeInfo);
     printf("        ->vbeMode           :   %d\n", mbi->vbeMode);
     printf("        ->vbeInterfaceSeg   :   %d\n", mbi->vbeInterfaceSeg);
     printf("        ->vbeInterfaceOff   :   %d\n", mbi->vbeInterfaceOff);
     printf("        ->vbeInterfaceLen   :   %d\n", mbi->vbeInterfaceLen);

  }
  printf("MBI_FLAGS 12 - 31 ignored and omitted\n");
}

/* --------------------------------------------------------------------- */

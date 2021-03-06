include ../../Makefile.common

KERNEL_SPACE    = 0xc0000000      # Kernel space starts at 3GB
KERNEL_LOAD     = 0x00100000      # Kernel loads at 1MB

INCPATH = -I ../../winio -I ../../mimg
CCDEFS  = -DKERNEL_SPACE=${KERNEL_SPACE} \
          -DKERNEL_LOAD=${KERNEL_LOAD}

LIBPATH = -L ../../winio -lwinio
LDDEFS  = --defsym KERNEL_SPACE=${KERNEL_SPACE} \
          --defsym KERNEL_LOAD=${KERNEL_LOAD}

all: kernel

#----------------------------------------------------------------------------
# A winio protected mode kernel that uses paging.

KOBJS	= init.o kernel.o paging.o initpic.o
kernel: ${KOBJS} kernel.ld
	$(LD) ${LDDEFS} -T kernel.ld -o kernel ${KOBJS} ${LIBPATH} \
		--print-map > kernel.map
	strip kernel

init.o:		init.S memory.h
kernel.o:	kernel.c context.h paging.h memory.h
paging.o:	paging.c paging.h memory.h

initpic.o: init.s
	$(CC) -Wa,-alsm=initpic.lst -c -o initpic.o initpic.s

.c.o:
	$(CC) ${CCOPTS} ${INCPATH} ${CCDEFS} -o $*.o -c $*.c

.S.o:
	$(CC) -Wa,-alsm=$*.lst ${INCPATH} ${CCDEFS} -o $*.o -c $*.S

.PHONY: twiddletests
twiddletests: twiddletests.c memory.h
	gcc -o twiddletests twiddletests.c memory.h
	./twiddletests
#----------------------------------------------------------------------------
# tidy up after ourselves ...
clean:
	-rm -r kernel twiddletests *.o *.lst *.map

#----------------------------------------------------------------------------

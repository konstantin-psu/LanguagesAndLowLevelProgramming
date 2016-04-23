#----------------------------------------------------------------------------
include Makefile.common

.phony: all run libs clean

#BOOT = hello
#BOOT = bootinfo
#BOOT = example-mimg
#BOOT = example-gdt
#BOOT = example-idt
BOOT = paging1

all:	libs
	make -C ${BOOT}

run:	libs
	make -C ${BOOT} run

all_targets: libs
	make -C hello
	make -C simpleio
	make -C bootinfo
	make -C mimg
	make -C example-mimg
	make -C example-gdt
	make -C example-idt
	make -C paging1

libs:
	make -C simpleio
	make -C mimg

clean:
	make -C hello              clean
	make -C simpleio           clean
	make -C bootinfo           clean
	make -C mimg               clean
	make -C example-mimg       clean
	make -C example-gdt        clean
	make -C example-idt        clean
	make -C paging1            clean

#----------------------------------------------------------------------------

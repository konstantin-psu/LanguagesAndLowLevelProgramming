#----------------------------------------------------------------------------
include Makefile.common

.phony: all run libs clean

#BOOT = hello
#BOOT = bootinfo
#BOOT = example-mimg
#BOOT = example-gdt
#BOOT = example-idt
BOOT = caps

all:	libs
	make -C hello
	make -C bootinfo
	make -C example-mimg
	make -C example-gdt
	make -C example-idt
	make -C paging1

run:	libs
	make -C ${BOOT} run

all_targets: libs
	make -C hello
	make -C bootinfo
	make -C example-mimg
	make -C example-gdt
	make -C example-idt
	make -C paging1
	make -C pork
	make -C caps


libs:
	make -C simpleio
	make -C mimg
	make -C winio
	make -C userio
	make -C mimg
	make -C capio

clean:
	make -C hello              clean
	make -C simpleio           clean
	make -C bootinfo           clean
	make -C mimg               clean
	make -C example-mimg       clean
	make -C example-gdt        clean
	make -C example-idt        clean
	make -C paging1            clean
	make -C winio              clean
	make -C userio             clean
	make -C pork               clean

#----------------------------------------------------------------------------

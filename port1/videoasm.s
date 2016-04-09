
	.equ	ROWS, 		25		# number of rows
	.equ	COLS, 		80		# number of columns
	.equ	COLS2, 		160		# number of columns

	.equ	CHARBYTES,	2		# total #bytes per char
	.equ	ROWBYTES,	COLS*CHARBYTES	# total #bytes per row
	.equ	SCREENBYTES,	ROWS*ROWBYTES	# total #bytes per screen

	.equ	SPACE,		0x20		# blank space
	.equ	NEWLINE,	'\n'		# newline character
	.equ	NEWLINEFEED, 0x0D		# newline character
	.equ	BACKSPACE,	 0x7F		# newline character

	.equ	DEFAULT_ATTR, 	0x2e		# PSU Green
	.equ	DEFAULT_ATTR2, 	0x5e		# PSU Green

	.data

        # Reserve space for a video ram frame buffer with
        # 25 rows; 80 columns; and one code and one attribute
        # byte per character.
	# .globl  video
	.align	4
# video:	.space	SCREENBYTES

        # Some variables to hold the current row, column, and
	# video attribute:
	.align	4
.globl row
row:	.long	0		# we will only use the least significant
.globl col
col:	.long	0		# bytes of these variables
attr:	.long	0

	.text

	# Clear the screen, setting all characters to SPACE
    # using the DEFAULT_ATTR attribute.
    # Mostly copied from class example
	.globl	cls
cls:	pushl	%ebp
	    movl	%esp, %ebp

        # save registers
        pushl %ecx 
        pushl %edx

        movl $(SCREENBYTES/4), %ecx
        movl video, %edx           # need $ sign to read address of video
        .equ HALF, (DEFAULT_ATTR<<8) | SPACE  # third byte color, fourth byte space
        .equ FULL, (HALF<<16) | HALF # first and third byte color, second and fourth byte spaces

1:      movl $FULL, (%edx)
        addl $4, %edx
        decl %ecx
        jnz 1b 

        # restore saved registers
        popl %edx
        popl %ecx

        movl	%ebp, %esp
        popl	%ebp
        ret

	# Set the video attribute for characters output using outc.
	.globl	setAttr
setAttr:pushl	%ebp
        movl	%esp, %ebp

        # save edx
        pushl %edx
        movl 8(%ebp), %edx  # copy argument to %edx
        movl %edx, attr  #  set attr variable

        # restore edx end exit
        popl %edx
        movl	%ebp, %esp
        popl	%ebp
        ret

        # Output a single character at the current row and col position
        # on screen, advancing the cursor coordinates and scrolling the
        # screen as appropriate.  Special treatment is provided for
            # NEWLINE characters, moving the "cursor" to the start of the
        # "next line".
        .globl	outc
outc:	pushl	%ebp
        movl	%esp, %ebp

        pushl %ecx  # Save all used registers  
        pushl %edx  # Save all used registers
        pushl %eax  # Save all used registers
        pushl %ebx  # Save all used registers
        pushl %esi  # Save all used registers

        movl $((SCREENBYTES-ROWBYTES)/4), %ecx # number of bytes left

        movl video, %edx   # no need "$" like in vram, since using pointers 

        movl attr, %eax  #copy attr to %eax

        movb 8(%ebp), %ah  # save char to ah

        cmpb $NEWLINE, %ah  # Test if new line
        je newLine         # do something about it

calculateOffset:
        # should have done it with leal
        movl row, %ebx
        imull $ROWBYTES, %ebx
        addl col, %ebx  # no ebx contains correct offset
        addl %ebx, %edx
        movb %ah, (%edx)  # output letter
        movb %al, 1(%edx) # output attr
        incl col          # increase col by 2 since we are using it to calculate per-byte offset
        incl col

        
newLineTest: # test if now out of boundaries, if so go to next line
        cmpl $COLS2, col 
        jl exitC # so far so good, exit
        
newLine: 
        incl row # go plus one row
        movl $0, col # reset col

test:
        cmpl $ROWS, row  # check if spilled out
        je setBack       # if yes scroll one line
        jmp exitC        # all good just exit
        

setBack:
        decl row        
        jmp copyAll

    
# Copy subroutine START
copyAll:
        movl ROWBYTES(%edx), %esi  # Save current + nextrow value
        movl %esi, (%edx)          # Copy next value to current position
        addl $4, %edx              # Advance
        decl %ecx                  # Decrement number of bytes left
        jnz copyAll                # if not zero copy more

        movl    $(ROWBYTES/4), %ecx #reset last line to empty

2:      movl $FULL, (%edx)         # Default attribute
        addl $4, %edx
        decl %ecx
        jnz 2b 
        
# Copy subroutine END

exitC:

        # restore all saved registers
        popl %esi
        popl %ebx
        popl %eax
        popl %edx
        popl %ecx

        movl	%ebp, %esp
        popl	%ebp
        ret



        # Output an unsigned numeric value as a sequence of 8 hex digits.
        .globl	outhex

        # Quite useful for baremetal debugging
outhex:	pushl	%ebp
        movl	%esp, %ebp
        
        # save all used registers
        pushl %edx
        pushl %ecx
        pushl %eax

        movl 8(%ebp), %edx # copy argument to edx
        movl $8, %ecx      # use %ecx as a counter to iterate over 8 bytes

loopOut:
        xorl %eax, %eax   # reset eax
        movb %dl, %al     # move first byte of the numeric value to al

        # this is done to leave only first half byte in eax
        shll $28, %eax
        shrl $28, %eax

        cmpl $0xA, %eax  # test if byte greater than 0xA
        jl printNumber   # if not then output numeric value
       
        addl $55, %eax   # else add 55 to output ASCII letter

        # Save final value to the stack
        # This is needed to account for the little endian problem
        # If print out right away bytes will be in reverse order
        pushl %eax      

        jmp testEndOutLoop

printNumber:
        addl $48, %eax
        pushl %eax

testEndOutLoop:
        decl %ecx  # if 0 - all good, exit
        jz exitOut
        shrl $4, %edx # else set dl to the next half bit
        jmp loopOut

exitOut:
        movl $8, %ecx # now need to output all saved numeric values
1:      
        popl %eax  # get the value to output
        pushl %ecx # save counter
        pushl %eax # put value back on the stack to pass as an argument to outc
        call outc
        popl %eax  # remove argument
        popl %ecx  # restore ecx
        decl %ecx
        jnz 1b

        # restore all saved registers
        popl %eax
        popl %ecx
        popl %edx

        movl	%ebp, %esp
        popl	%ebp
        ret



	.equ	ROWS, 		25		# number of rows
	.equ	COLS, 		80		# number of columns
	.equ	COLS2, 		160		# number of columns

	.equ	CHARBYTES,	2		# total #bytes per char
	.equ	ROWBYTES,	COLS*CHARBYTES	# total #bytes per row
	.equ	SCREENBYTES,	ROWS*ROWBYTES	# total #bytes per screen

	.equ	SPACE,		0x20		# blank space
	.equ	NEWLINE,	'\n'		# newline character

	.equ	DEFAULT_ATTR, 	0x2e		# PSU Green
	.equ	DEFAULT_ATTR2, 	0x5e		# PSU Green

	.data

        # Reserve space for a video ram frame buffer with
        # 25 rows; 80 columns; and one code and one attribute
        # byte per character.
	.globl  video
	.align	4
#video:	.space	SCREENBYTES

        # Some variables to hold the current row, column, and
	# video attribute:
	.align	4
row:	.long	0		# we will only use the least significant
col:	.long	0		# bytes of these variables
attr:	.long	0

	.text

	# Clear the screen, setting all characters to SPACE
        # using the DEFAULT_ATTR attribute.
	.globl	cls
cls:	pushl	%ebp
	    movl	%esp, %ebp
	    # Fill me in!

        movl $(SCREENBYTES/4), %ecx
        movl $video, %edx           # need $ sign to read address of video
        .equ HALF, (DEFAULT_ATTR<<8) | SPACE  # third byte color, fourth byte space
        .equ FULL, (HALF<<16) | HALF # first and third byte color, second and fourth byte spaces

1:      movl $FULL, (%edx)
        addl $4, %edx
        decl %ecx
        jnz 1b 



        movl	%ebp, %esp
        popl	%ebp
        ret

	# Set the video attribute for characters output using outc.
	.globl	setAttr
setAttr:pushl	%ebp
        movl	%esp, %ebp
        # Fill me in!

        movl 8(%ebp), %edx  # save the argument to %al char = 1 Byte
        movl %edx, attr  # save the argument to %al char = 1 Byte

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

        movl $((SCREENBYTES-ROWBYTES)/4), %ecx # number of bytes left

        movl $video, %edx           # need $ sign to read address of video

        movl attr, %eax
        shll  $8, %eax
        movb 8(%ebp), %al  # save the argument to %al char = 1 Byte

        cmpb $NEWLINE, %al  # Test if new line

        je newLine

        movl row, %ebx
        imull $ROWBYTES, %ebx
        addl col, %ebx
        addl %ebx, %edx
        movb %al, (%edx)
        movb %ah, 1(%edx)
        incl col
        incl col

        jmp newLineTest

newLineTest:
        cmpl $COLS2, col
        jl exitC
        
newLine: 
        incl row
        movl $0, col

test:
        cmpl $ROWS, row
        je setBack
        jmp exitC
        

setBack:
        movl $0, col
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

2:      movl $FULL, (%edx)         # Green Space
        addl $4, %edx
        decl %ecx
        jnz 2b 
        
# Copy subroutine END

exitC:
        movl	%ebp, %esp
        popl	%ebp
        ret
        # Output an unsigned numeric value as a sequence of 8 hex digits.
        .globl	outhex


outhex:	pushl	%ebp
        movl	%esp, %ebp
        
        movl 8(%ebp), %edx
        movl $8, %ecx

loopOut:
        xorl %eax, %eax
        movb %dl, %al
        shll $28, %eax
        shrl $28, %eax
        cmpl $0xA, %eax
        jl printNumber
       
        addl $55, %eax
        pushl %eax
        jmp testEndOutLoop

printNumber:
        addl $48, %eax
        pushl %eax

testEndOutLoop:
        decl %ecx
        jz exitOut
        shrl $4, %edx
        jmp loopOut

exitOut:
        movl $8, %ecx
1:      
        popl %eax
        pushl %ecx
        pushl %eax
        call outc
        popl %eax
        popl %ecx
        decl %ecx
        jnz 1b


        movl	%ebp, %esp
        popl	%ebp
        ret


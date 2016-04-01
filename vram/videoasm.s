
	.equ	ROWS, 		25		# number of rows
	.equ	COLS, 		80		# number of columns

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
video:	.space	SCREENBYTES

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
        decl %ecx
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
        # Fill me in!

        movl $((SCREENBYTES-ROWBYTES)/4), %ecx

        movl $video, %edx           # need $ sign to read address of video
        xorl %eax, %eax


        orl (%edx), %eax
        movb 8(%ebp), %al

        cmpb $NEWLINE, %al
        je copyAll
        # .equ HALF2, (DEFAULT_ATTR2<<8) | %al  # third byte color, fourth byte space

        # orw DEFAULT_ATTR2, %ah

        addl col, %edx
        movb %al, (%edx)
        incl col
        incl col

        jmp exitC
        # incl col
        # cmpl col, $COLS
        # movl $col, video

    
copyAll:      movl ROWBYTES(%edx), %esi
        movl %esi, (%edx)
        addl $4, %edx
        decl %ecx
        jnz copyAll

        movl    $(ROWBYTES/4), %ecx
2:      movl $FULL, (%edx)
        addl $4, %edx
        decl %ecx
        jnz 2b 
        
        subl $COLS, col

exitC:
        movl	%ebp, %esp
        popl	%ebp
        ret
        # Output an unsigned numeric value as a sequence of 8 hex digits.
        .globl	outhex
outhex:	pushl	%ebp
        movl	%esp, %ebp
        # Fill me in!
        movl	%ebp, %esp
        popl	%ebp
        ret


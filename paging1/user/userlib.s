	.text
	.globl	entry
entry:	leal	stack, %esp
	jmp	cmain

	.data
	.space	4096		# User stack
stack:

	# System call to print a character in the kernel's window
	.globl	kputc
kputc:	pushl	%eax
	mov	8(%esp), %eax
	int	$128
	popl	%eax
	ret

    .globl hold
hold:
    int $132
    ret


#-------------------------------------------------------------------------
# Set the video color attribute.

#         .globl  setAttr
# setAttr:pushl   %eax
#         mov     8(%esp), %eax
#         int     $0x82
#         popl    %eax
#         ret

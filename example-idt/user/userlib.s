	.text
	.globl	entry
entry:	leal	stack, %esp
	jmp	cmain

	.data
	.space	4096		# User stack
stack:

	# System call to print a character in the kernel's window
    .text
	.globl	kputc
kputc:	pushl	%eax
	mov	8(%esp), %eax
	int	$0x80
	popl	%eax
	ret

    .globl yield
yield:
    int $0x81
    ret

    .globl hold
hold:
    int $0x82
    ret

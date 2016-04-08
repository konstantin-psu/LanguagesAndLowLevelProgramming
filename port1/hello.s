	.file	"hello.c"
	.text
	.globl	cls
	.type	cls, @function
cls:
.LFB0:
	.cfi_startproc
	pushl	%edi
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	pushl	%esi
	.cfi_def_cfa_offset 12
	.cfi_offset 6, -12
	pushl	%ebx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	$0, %esi
	jmp	.L2
.L5:
	leal	(%ebx,%eax), %ecx
	movl	video, %edi
	movb	$32, (%edi,%ecx,2)
	movl	video, %edi
	movb	%dl, 1(%edi,%ecx,2)
	addl	$1, %eax
	addl	$1, %edx
	cmpl	$80, %eax
	jne	.L5
	addl	$1, %esi
	cmpl	$25, %esi
	je	.L4
.L2:
	leal	12(%esi), %edx
	movl	$0, %eax
	leal	(%esi,%esi,4), %ebx
	sall	$4, %ebx
	jmp	.L5
.L4:
	movl	$0, ypos
	movl	$0, xpos
	popl	%ebx
	.cfi_restore 3
	.cfi_def_cfa_offset 12
	popl	%esi
	.cfi_restore 6
	.cfi_def_cfa_offset 8
	popl	%edi
	.cfi_restore 7
	.cfi_def_cfa_offset 4
	ret
	.cfi_endproc
.LFE0:
	.size	cls, .-cls
	.globl	putchar
	.type	putchar, @function
putchar:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	pushl	%edi
	.cfi_def_cfa_offset 12
	.cfi_offset 7, -12
	pushl	%esi
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushl	%ebx
	.cfi_def_cfa_offset 20
	.cfi_offset 3, -20
	subl	$4, %esp
	.cfi_def_cfa_offset 24
	movl	24(%esp), %ecx
	cmpb	$13, %cl
	je	.L8
	cmpb	$10, %cl
	je	.L8
	movl	ypos, %edx
	movl	xpos, %eax
	leal	0(,%edx,4), %ebx
	leal	(%ebx,%edx), %edi
	sall	$4, %edi
	addl	%eax, %edi
	movl	video, %esi
	movb	%cl, (%esi,%edi,2)
	movl	video, %edx
	movb	$12, 1(%edx,%edi,2)
	addl	$1, %eax
	movl	%eax, xpos
	cmpl	$79, %eax
	jle	.L7
.L8:
	movl	$0, xpos
	movl	ypos, %eax
	addl	$1, %eax
	cmpl	$24, %eax
	jg	.L10
	movl	%eax, ypos
	jmp	.L7
.L10:
	movl	$24, ypos
	movl	$0, %edi
	jmp	.L11
.L14:
	movl	video, %edx
	movl	(%esp), %ebx
	leal	(%ebx,%eax), %ecx
	movzbl	(%edx,%ecx,2), %ebp
	leal	(%esi,%eax), %ecx
	movl	%ebp, %ebx
	movb	%bl, (%edx,%ecx,2)
	movl	video, %edx
	movl	(%esp), %ebx
	leal	(%ebx,%eax), %ebp
	movzbl	1(%edx,%ebp,2), %ebp
	movl	%ebp, %ebx
	movb	%bl, 1(%edx,%ecx,2)
	addl	$1, %eax
	cmpl	$80, %eax
	jne	.L14
	addl	$1, %edi
	cmpl	$24, %edi
	je	.L13
.L11:
	movl	$0, %eax
	leal	5(%edi,%edi,4), %ebx
	sall	$4, %ebx
	movl	%ebx, (%esp)
	leal	(%edi,%edi,4), %esi
	sall	$4, %esi
	jmp	.L14
.L13:
	movl	video, %edx
	movl	$0, %eax
.L15:
	movb	$32, 3840(%edx,%eax,2)
	movb	$12, 3841(%edx,%eax,2)
	addl	$1, %eax
	cmpl	$80, %eax
	jne	.L15
.L7:
	addl	$4, %esp
	.cfi_def_cfa_offset 20
	popl	%ebx
	.cfi_restore 3
	.cfi_def_cfa_offset 16
	popl	%esi
	.cfi_restore 6
	.cfi_def_cfa_offset 12
	popl	%edi
	.cfi_restore 7
	.cfi_def_cfa_offset 8
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa_offset 4
	ret
	.cfi_endproc
.LFE1:
	.size	putchar, .-putchar
	.globl	puts
	.type	puts, @function
puts:
.LFB2:
	.cfi_startproc
	pushl	%ebx
	.cfi_def_cfa_offset 8
	.cfi_offset 3, -8
	subl	$4, %esp
	.cfi_def_cfa_offset 12
	movl	12(%esp), %ebx
	movzbl	(%ebx), %eax
	testb	%al, %al
	je	.L17
.L19:
	addl	$1, %ebx
	movsbl	%al, %eax
	movl	%eax, (%esp)
	call	putchar
	movzbl	(%ebx), %eax
	testb	%al, %al
	jne	.L19
.L17:
	addl	$4, %esp
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_restore 3
	.cfi_def_cfa_offset 4
	ret
	.cfi_endproc
.LFE2:
	.size	puts, .-puts
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"hhhh   hhhh\n"
.LC1:
	.string	" hh    hhh        lll lll\n"
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align 4
.LC2:
	.string	" hh    hh   eeee  ll  ll   oooo\n"
	.align 4
.LC3:
	.string	" hhhhhhhh  ee  ee ll  ll  oo  oo\n"
	.align 4
.LC4:
	.string	" hh    hh eeeeeee ll  ll oo   oo\n"
	.align 4
.LC5:
	.string	" hh    hh  ee     ll  ll oo  oo\n"
	.align 4
.LC6:
	.string	"hhh   hhhh  eeee  ll  ll  oooo\n"
	.section	.rodata.str1.1
.LC7:
	.string	"\n"
.LC8:
	.string	"    K e r n e l   W o r l d\n"
	.text
	.globl	hello
	.type	hello, @function
hello:
.LFB3:
	.cfi_startproc
	pushl	%ebx
	.cfi_def_cfa_offset 8
	.cfi_offset 3, -8
	subl	$4, %esp
	.cfi_def_cfa_offset 12
	call	cls
	movl	$2, %ebx
.L23:
	movl	$.LC0, (%esp)
	call	puts
	movl	$.LC1, (%esp)
	call	puts
	movl	$.LC2, (%esp)
	call	puts
	movl	$.LC3, (%esp)
	call	puts
	movl	$.LC4, (%esp)
	call	puts
	movl	$.LC5, (%esp)
	call	puts
	movl	$.LC6, (%esp)
	call	puts
	movl	$.LC7, (%esp)
	call	puts
	movl	$.LC8, (%esp)
	call	puts
	subl	$1, %ebx
	jne	.L23
	addl	$4, %esp
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_restore 3
	.cfi_def_cfa_offset 4
	ret
	.cfi_endproc
.LFE3:
	.size	hello, .-hello
	.local	ypos
	.comm	ypos,4,4
	.local	xpos
	.comm	xpos,4,4
	.globl	video
	.data
	.align 4
	.type	video, @object
	.size	video, 4
video:
	.long	753664
	.ident	"GCC: (Ubuntu 4.8.4-2ubuntu1~14.04.1) 4.8.4"
	.section	.note.GNU-stack,"",@progbits

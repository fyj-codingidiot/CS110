	.file	"solution.c"
	.text
	.comm	a,40,32
	.comm	b,48,32
	.globl	len1
	.data
	.align 4
	.type	len1, @object
	.size	len1, 4
len1:
	.long	10
	.globl	len2
	.align 4
	.type	len2, @object
	.size	len2, 4
len2:
	.long	12
	.text
	.globl	search
	.type	search, @function
search:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	len1(%rip), %eax
	cmpl	%eax, -4(%rbp)
	jne	.L2
	movl	$0, %eax
	jmp	.L3
.L2:
	movl	len2(%rip), %eax
	cmpl	%eax, -8(%rbp)
	jne	.L4
	movl	$0, %eax
	jmp	.L3
.L4:
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	a(%rip), %rax
	movl	(%rdx,%rax), %edx
	movl	-8(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rcx
	leaq	b(%rip), %rax
	movl	(%rcx,%rax), %eax
	cmpl	%eax, %edx
	jne	.L5
	movl	-8(%rbp), %eax
	leal	1(%rax), %edx
	movl	-4(%rbp), %eax
	addl	$1, %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	search
	addl	$1, %eax
	jmp	.L3
.L5:
	movl	$0, %eax
.L3:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	search, .-search
	.globl	func
	.type	func, @function
func:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$0, -16(%rbp)
	movl	$0, -12(%rbp)
	jmp	.L7
.L11:
	movl	$0, -8(%rbp)
	jmp	.L8
.L10:
	movl	-8(%rbp), %edx
	movl	-12(%rbp), %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	search
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	cmpl	-16(%rbp), %eax
	jle	.L9
	movl	-4(%rbp), %eax
	movl	%eax, -16(%rbp)
.L9:
	addl	$1, -8(%rbp)
.L8:
	movl	len2(%rip), %eax
	cmpl	%eax, -8(%rbp)
	jl	.L10
	addl	$1, -12(%rbp)
.L7:
	movl	len1(%rip), %eax
	cmpl	%eax, -12(%rbp)
	jl	.L11
	movl	-16(%rbp), %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	func, .-func
	.section	.rodata
.LC0:
	.string	"%d"
	.text
	.globl	main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$0, -16(%rbp)
	jmp	.L14
.L15:
	leaq	-20(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	__isoc99_scanf@PLT
	movl	-20(%rbp), %eax
	movl	-16(%rbp), %edx
	movslq	%edx, %rdx
	leaq	0(,%rdx,4), %rcx
	leaq	a(%rip), %rdx
	movl	%eax, (%rcx,%rdx)
	addl	$1, -16(%rbp)
.L14:
	movl	len1(%rip), %eax
	cmpl	%eax, -16(%rbp)
	jl	.L15
	movl	$0, -16(%rbp)
	jmp	.L16
.L17:
	leaq	-20(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	__isoc99_scanf@PLT
	movl	-20(%rbp), %eax
	movl	-16(%rbp), %edx
	movslq	%edx, %rdx
	leaq	0(,%rdx,4), %rcx
	leaq	b(%rip), %rdx
	movl	%eax, (%rcx,%rdx)
	addl	$1, -16(%rbp)
.L16:
	movl	len2(%rip), %eax
	cmpl	%eax, -16(%rbp)
	jl	.L17
	movl	$0, %eax
	call	func
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
	movq	-8(%rbp), %rsi
	xorq	%fs:40, %rsi
	je	.L19
	call	__stack_chk_fail@PLT
.L19:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:

	.file	"fix_trans.c"
	.text
	.p2align 4,,15
.globl t_compare
	.type	t_compare, @function
t_compare:
.LFB32:
	.cfi_startproc
	movzwl	(%rsi), %edx
	movl	$1, %eax
	cmpw	%dx, (%rdi)
	jle	.L7
	rep
	ret
	.p2align 4,,10
	.p2align 3
.L7:
	setge	%al
	movzbl	%al, %eax
	subl	$1, %eax
	ret
	.cfi_endproc
.LFE32:
	.size	t_compare, .-t_compare
	.p2align 4,,15
.globl set_tag_nine
	.type	set_tag_nine, @function
set_tag_nine:
.LFB54:
	.cfi_startproc
	movq	24(%rsi), %rdx
	movq	16(%rsi), %rax
	movabsq	$-3689348814741910323, %r9
	movq	24(%rdx), %rcx
	movq	16(%rdi), %rdx
	xorl	%edi, %edi
	subl	$1, %ecx
	subl	48(%rax), %ecx
	movq	8(%rdx), %r8
	subl	56(%rax), %ecx
	.p2align 4,,10
	.p2align 3
.L9:
	movq	%rcx, %rax
	mulq	%r9
	shrq	$3, %rdx
	leaq	(%rdx,%rdx,4), %rax
	addq	%rax, %rax
	subq	%rax, %rcx
	movzbl	dig.4216(%rcx), %eax
	movq	%rdx, %rcx
	movb	%al, (%r8,%rdi)
	addq	$1, %rdi
	testq	%rdx, %rdx
	jne	.L9
	movq	16(%rsi), %rdx
	addq	$32, %rdx
	movslq	24(%rdx), %rax
	subq	%rdi, %rax
	addq	16(%rdx), %rax
	leaq	(%r8,%rdi), %rdi
	.p2align 4,,10
	.p2align 3
.L10:
	movzbl	-1(%rdi), %edx
	subq	$1, %rdi
	movb	%dl, (%rax)
	addq	$1, %rax
	cmpq	%r8, %rdi
	jne	.L10
	rep
	ret
	.cfi_endproc
.LFE54:
	.size	set_tag_nine, .-set_tag_nine
	.p2align 4,,15
.globl get_message
	.type	get_message, @function
get_message:
.LFB61:
	.cfi_startproc
	movq	24(%rdi), %rax
	ret
	.cfi_endproc
.LFE61:
	.size	get_message, .-get_message
	.p2align 4,,15
.globl get_fixy_message
	.type	get_fixy_message, @function
get_fixy_message:
.LFB49:
	.cfi_startproc
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movq	(%rdi), %rdi
	movw	%si, 12(%rsp)
	leaq	12(%rsp), %rsi
	call	rb_tree_get_val
	movq	%rax, %rdi
	xorl	%eax, %eax
	testq	%rdi, %rdi
	je	.L18
	call	get_stuff
.L18:
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE49:
	.size	get_fixy_message, .-get_fixy_message
	.p2align 4,,15
.globl translate_fix_msg
	.type	translate_fix_msg, @function
translate_fix_msg:
.LFB48:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	pushq	%r14
	.cfi_def_cfa_offset 24
	pushq	%r13
	.cfi_def_cfa_offset 32
	movq	%rsi, %r13
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	.cfi_offset 15, -16
	pushq	%r12
	.cfi_def_cfa_offset 40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	movq	%rdi, %rbp
	.cfi_offset 6, -48
	.cfi_offset 12, -40
	pushq	%rbx
	.cfi_def_cfa_offset 56
	movl	%edx, %ebx
	.cfi_offset 3, -56
	subq	$24, %rsp
	.cfi_def_cfa_offset 80
	movq	8(%rdi), %rdi
	leaq	12(%rsp), %rsi
	movq	%rcx, (%rsp)
	movw	%r8w, 12(%rsp)
	call	rb_tree_get_val
	testq	%rax, %rax
	movq	%rax, %r14
	je	.L28
	leaq	24(%rbp), %r15
	movq	%r13, %rsi
	jmp	.L27
	.p2align 4,,10
	.p2align 3
.L32:
	subl	$1, %ebx
	addq	$1, %r13
	testl	%ebx, %ebx
	jle	.L28
.L27:
	cmpb	$61, 0(%r13)
	jne	.L32
	movq	%r13, %r12
	.p2align 4,,10
	.p2align 3
.L24:
	subl	$1, %ebx
	addq	$1, %r12
	testl	%ebx, %ebx
	jle	.L29
	cmpb	$1, (%r12)
	jne	.L24
.L29:
	movl	%r13d, %edx
	movq	%r15, %rcx
	movq	%r14, %rdi
	subl	%esi, %edx
	call	find_n_fill
	testl	%eax, %eax
	jle	.L26
	movq	24(%rbp), %rax
	leaq	1(%r13), %rdx
	movq	(%rsp), %rdi
	movl	%r12d, %ecx
	subl	%edx, %ecx
	movl	(%rax), %esi
	call	reset_rom_field
.L26:
	leaq	1(%r12), %rsi
	subl	$1, %ebx
	testl	%ebx, %ebx
	movq	%rsi, %r13
	jg	.L27
.L28:
	addq	$24, %rsp
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE48:
	.size	translate_fix_msg, .-translate_fix_msg
	.p2align 4,,15
	.type	clear_pg, @function
clear_pg:
.LFB44:
	.cfi_startproc
	testb	$1, %dil
	movl	$4096, %edx
	jne	.L41
	testb	$2, %dil
	jne	.L42
.L35:
	testb	$4, %dil
	jne	.L43
.L36:
	movl	%edx, %ecx
	xorl	%eax, %eax
	shrl	$3, %ecx
	testb	$4, %dl
	mov	%ecx, %ecx
	rep stosq
	je	.L37
	movl	$0, (%rdi)
	addq	$4, %rdi
.L37:
	testb	$2, %dl
	je	.L38
	movw	$0, (%rdi)
	addq	$2, %rdi
.L38:
	andl	$1, %edx
	je	.L40
	movb	$0, (%rdi)
.L40:
	rep
	ret
	.p2align 4,,10
	.p2align 3
.L41:
	movb	$0, (%rdi)
	addq	$1, %rdi
	movw	$4095, %dx
	testb	$2, %dil
	je	.L35
	.p2align 4,,10
	.p2align 3
.L42:
	movw	$0, (%rdi)
	addq	$2, %rdi
	subl	$2, %edx
	testb	$4, %dil
	je	.L36
	.p2align 4,,10
	.p2align 3
.L43:
	movl	$0, (%rdi)
	subl	$4, %edx
	addq	$4, %rdi
	jmp	.L36
	.cfi_endproc
.LFE44:
	.size	clear_pg, .-clear_pg
	.p2align 4,,15
.globl reset_object
	.type	reset_object, @function
reset_object:
.LFB40:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	movq	(%rdi), %rdi
	movq	%rsi, %rbx
	.cfi_offset 3, -16
	leaq	36(%rsi), %rsi
	call	rb_tree_get_val
	testq	%rax, %rax
	je	.L47
	movq	24(%rbx), %rdx
	movslq	32(%rbx), %rax
	movl	$9, %esi
	movq	(%rbx), %rdi
	addq	16(%rdx), %rax
	movq	%rax, 24(%rdx)
	call	sti_tree_get_val
	cwtl
	testl	%eax, %eax
	jle	.L46
	cltq
	movl	$48, %esi
	salq	$5, %rax
	addq	16(%rbx), %rax
	movslq	24(%rax), %rdx
	movq	16(%rax), %rdi
	call	memset
.L46:
	movq	(%rbx), %rdi
	movl	$34, %esi
	call	sti_tree_get_val
	cwtl
	testl	%eax, %eax
	jle	.L47
	cltq
	movl	$48, %esi
	salq	$5, %rax
	addq	16(%rbx), %rax
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	movslq	24(%rax), %rdx
	movq	16(%rax), %rdi
	jmp	memset
	.p2align 4,,10
	.p2align 3
.L47:
	.cfi_restore_state
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE40:
	.size	reset_object, .-reset_object
	.p2align 4,,15
.globl t_destroy
	.type	t_destroy, @function
t_destroy:
.LFB31:
	.cfi_startproc
	jmp	free
	.cfi_endproc
.LFE31:
	.size	t_destroy, .-t_destroy
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"r"
.LC1:
	.string	"Alignment was not power of 2"
.LC2:
	.string	"No memory for align."
	.text
	.p2align 4,,15
	.type	get_file_contents, @function
get_file_contents:
.LFB37:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsi, %rbp
	.cfi_offset 6, -16
	movl	$.LC0, %esi
	pushq	%rbx
	.cfi_def_cfa_offset 24
	subq	$24, %rsp
	.cfi_def_cfa_offset 48
	movq	$0, 8(%rsp)
	.cfi_offset 3, -24
	call	fopen
	testq	%rax, %rax
	movq	%rax, %rbx
	je	.L51
	xorl	%esi, %esi
	movl	$2, %edx
	movq	%rax, %rdi
	call	fseek
	movq	%rbx, %rdi
	call	ftell
	movq	%rbx, %rdi
	movq	%rax, 0(%rbp)
	call	rewind
	movq	0(%rbp), %rdx
	leaq	8(%rsp), %rdi
	movl	$8, %esi
	addq	$1, %rdx
	call	posix_memalign
	cmpl	$22, %eax
	je	.L57
	cmpl	$12, %eax
	je	.L58
	movq	0(%rbp), %rdx
	movq	8(%rsp), %rdi
	xorl	%esi, %esi
	addq	$1, %rdx
	call	memset
.L53:
	movq	0(%rbp), %rdx
	movq	8(%rsp), %rdi
	movq	%rbx, %rcx
	movl	$1, %esi
	call	fread
	cmpq	%rax, 0(%rbp)
	je	.L55
	movq	8(%rsp), %rdi
	call	free
	movq	$0, 8(%rsp)
.L55:
	movq	%rbx, %rdi
	call	fclose
.L51:
	movq	8(%rsp), %rax
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L58:
	.cfi_restore_state
	movl	$.LC2, %edi
	call	puts
	jmp	.L53
	.p2align 4,,10
	.p2align 3
.L57:
	movl	$.LC1, %edi
	call	puts
	.p2align 4,,3
	jmp	.L53
	.cfi_endproc
.LFE37:
	.size	get_file_contents, .-get_file_contents
	.p2align 4,,15
.globl create_translator
	.type	create_translator, @function
create_translator:
.LFB47:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	pushq	%r14
	.cfi_def_cfa_offset 24
	pushq	%r13
	.cfi_def_cfa_offset 32
	pushq	%r12
	.cfi_def_cfa_offset 40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	movq	%rdi, %rbx
	.cfi_offset 3, -56
	.cfi_offset 6, -48
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	.cfi_offset 15, -16
	movl	$1, %edi
	subq	$152, %rsp
	.cfi_def_cfa_offset 208
	movl	%esi, 96(%rsp)
	movl	$32, %esi
	movq	%rdx, 120(%rsp)
	movl	%ecx, 100(%rsp)
	call	calloc
	movl	$64, %edi
	movq	%rax, 112(%rsp)
	call	databuf_alloc
	movq	112(%rsp), %rdx
	movl	$ft_print, %r8d
	movl	$t_destroy, %esi
	movq	%r8, %rcx
	movl	$t_compare, %edi
	movq	%rax, 16(%rdx)
	movl	$t_info_destroy, %edx
	call	create_rb_tree
	movq	112(%rsp), %rcx
	movl	$ft_print, %r8d
	movl	$i_info_destroy, %edx
	movl	$t_destroy, %esi
	movl	$t_compare, %edi
	movq	%rax, (%rcx)
	movq	%r8, %rcx
	call	create_rb_tree
	movq	112(%rsp), %rdx
	movl	$4, %esi
	movl	$1, %edi
	movq	%rax, 8(%rdx)
	call	calloc
	movq	112(%rsp), %rcx
	movq	%rax, 24(%rcx)
	movslq	96(%rsp), %rax
	salq	$4, %rax
	leaq	-16(%rbx,%rax), %rax
	movq	%rax, 104(%rsp)
	.p2align 4,,10
	.p2align 3
.L81:
	movq	104(%rsp), %rax
	movl	$clear_pg, %esi
	movl	$32768, %edi
	subl	$1, 96(%rsp)
	xorl	%r13d, %r13d
	movq	(%rax), %rbx
	call	create_mem_glob
	movl	$2, %esi
	movl	$1, %edi
	movq	%rax, 48(%rsp)
	movq	$0, 128(%rsp)
	call	calloc
	leaq	128(%rsp), %rsi
	movq	%rbx, %rdi
	movq	%rax, 80(%rsp)
	call	get_file_contents
	movq	%rax, %rcx
	addq	128(%rsp), %rcx
	movq	%rax, 72(%rsp)
	.p2align 4,,10
	.p2align 3
.L61:
	xorl	%edx, %edx
	cmpb	$10, (%rax)
	sete	%dl
	addq	$1, %rax
	addl	%edx, %r13d
	cmpq	%rcx, %rax
	jne	.L61
	leal	0(%r13,%r13,4), %eax
	movl	$0, 36(%rsp)
	leal	104(,%rax,8), %eax
	cltq
	movq	%rax, 56(%rsp)
	movl	%r13d, %eax
	sall	$5, %eax
	cltq
	movq	%rax, 40(%rsp)
	addq	$40, %rax
	movq	%rax, 88(%rsp)
	.p2align 4,,10
	.p2align 3
.L80:
	movq	48(%rsp), %rdi
	call	get_stuff
	movq	88(%rsp), %rdi
	movq	%rax, %rbx
	movl	$0, 32(%rax)
	movq	128(%rsp), %rdx
	leaq	40(%rax), %rax
	movq	72(%rsp), %rsi
	leaq	(%rbx,%rdi), %rdi
	movq	%rax, 16(%rbx)
	movq	%rdi, 8(%rbx)
	call	memcpy
	movq	40(%rsp), %rdx
	movq	128(%rsp), %rax
	movq	56(%rsp), %rsi
	leaq	40(%rdx,%rax), %rdi
	leaq	(%rbx,%rdi), %rdi
	call	create_sti_tree
	movq	128(%rsp), %r12
	movq	56(%rsp), %rdx
	movq	%rax, 24(%rsp)
	movq	%rax, (%rbx)
	movq	24(%rsp), %rdi
	addq	%r12, %rdx
	addq	40(%rsp), %rdx
	leaq	40(%rbx,%rdx), %rax
	leaq	80(%rbx,%rdx), %rcx
	movq	%rcx, 8(%rax)
	movq	%rcx, 24(%rax)
	movq	%rcx, 16(%rax)
	movl	$4016, %ecx
	movq	%rax, 24(%rbx)
	subq	%rdx, %rcx
	movl	$1, 32(%rax)
	testq	%r12, %r12
	movq	%rcx, (%rax)
	movq	16(%rbx), %rax
	movq	8(%rbx), %rbp
	movq	%rax, 16(%rsp)
	jle	.L62
	xorl	%ecx, %ecx
	movq	%rbx, 64(%rsp)
	movq	%rbp, %r14
	movl	$0, 8(%rsp)
	xorl	%r15d, %r15d
	movl	%ecx, %ebx
	jmp	.L72
	.p2align 4,,10
	.p2align 3
.L98:
	cmpb	$44, %al
	je	.L63
	cmpb	$10, %al
	je	.L63
	subq	$1, %r12
	addq	$1, %rbp
	testq	%r12, %r12
	jle	.L97
.L72:
	movzbl	0(%rbp), %eax
	cmpb	$94, %al
	jne	.L98
.L63:
	testl	%r15d, %r15d
	je	.L99
.L65:
	cmpb	$44, %al
	je	.L100
.L66:
	cmpb	$94, %al
	.p2align 4,,4
	je	.L101
	cmpb	$10, %al
	.p2align 4,,5
	je	.L69
.L96:
	leaq	1(%rbp), %r14
.L67:
	subq	$1, %r12
	movq	%r14, %rbp
	testq	%r12, %r12
	jg	.L72
.L97:
	movq	64(%rsp), %rbx
	movq	16(%rbx), %rcx
	movq	(%rbx), %rdi
	movq	%rcx, 16(%rsp)
.L62:
	movl	$35, %esi
	call	sti_tree_get_val
	cwtl
	xorl	%edx, %edx
	testl	%eax, %eax
	jle	.L74
	cltq
	salq	$5, %rax
	addq	16(%rsp), %rax
	cmpl	$1, 24(%rax)
	movq	16(%rax), %rcx
	jle	.L75
	movsbw	1(%rcx), %ax
	movsbw	(%rcx), %dx
	sall	$8, %eax
	leal	(%rax,%rdx), %edx
	movswl	%dx, %edx
.L74:
	testl	%r13d, %r13d
	movl	%edx, 36(%rbx)
	je	.L76
	xorl	%ebp, %ebp
	xorl	%r12d, %r12d
	jmp	.L79
	.p2align 4,,10
	.p2align 3
.L77:
	movq	24(%rbx), %rdx
	movq	24(%rdx), %rdi
	movslq	24(%rax), %rdx
	call	memcpy
.L78:
	movq	24(%rbx), %rax
	movq	16(%rbx), %rdx
	addl	$1, %r12d
	movq	24(%rax), %rcx
	movq	%rcx, 16(%rdx,%rbp)
	movslq	24(%rdx,%rbp), %rdx
	addq	$32, %rbp
	addq	%rdx, 24(%rax)
	movq	24(%rax), %rdx
	movb	$1, (%rdx)
	addq	$1, %rdx
	cmpl	%r13d, %r12d
	movq	%rdx, 24(%rax)
	jge	.L76
.L79:
	movq	%rbp, %rax
	addq	16(%rbx), %rax
	movq	24(%rbx), %rdi
	movslq	8(%rax), %rdx
	movq	(%rax), %rsi
	call	databuf_memcpy
	movq	%rbp, %rax
	addq	16(%rbx), %rax
	movq	16(%rax), %rsi
	testq	%rsi, %rsi
	jne	.L77
	movslq	24(%rax), %rdx
	movq	24(%rbx), %rax
	movb	$48, %sil
	movq	24(%rax), %rdi
	call	memset
	jmp	.L78
	.p2align 4,,10
	.p2align 3
.L99:
	testl	%ebx, %ebx
	jne	.L65
	movslq	8(%rsp), %rax
	movl	%ebp, %edx
	xorl	%esi, %esi
	subl	%r14d, %edx
	movq	%r14, %rdi
	salq	$5, %rax
	addq	16(%rsp), %rax
	movl	%edx, 8(%rax)
	movq	%r14, (%rax)
	movl	$10, %edx
	call	strtol
	movswl	8(%rsp), %edx
	movq	24(%rsp), %rdi
	movswl	%ax, %esi
	call	sti_tree_insert
	movzbl	0(%rbp), %eax
	cmpb	$44, %al
	jne	.L66
.L100:
	leaq	1(%rbp), %r14
	movl	$1, %ebx
	jmp	.L67
	.p2align 4,,10
	.p2align 3
.L76:
	movq	24(%rbx), %rdi
	call	databuf_unread
	movq	80(%rsp), %rdx
	movl	%eax, 32(%rbx)
	movq	%rbx, %rsi
	movl	36(%rbx), %eax
	movq	48(%rsp), %rdi
	movw	%ax, (%rdx)
	call	return_stuff
	addl	$1, 36(%rsp)
	cmpl	$8, 36(%rsp)
	jne	.L80
	movq	112(%rsp), %rcx
	movq	48(%rsp), %rdx
	movq	80(%rsp), %rsi
	movq	(%rcx), %rdi
	call	rb_tree_insert
	movl	96(%rsp), %edx
	subq	$16, 104(%rsp)
	testl	%edx, %edx
	jne	.L81
	movslq	100(%rsp), %rax
	movq	120(%rsp), %rdx
	salq	$4, %rax
	leaq	-4(%rdx,%rax), %r14
	.p2align 4,,10
	.p2align 3
.L87:
	movzwl	(%r14), %ecx
	movq	-12(%r14), %rbx
	movl	$64, %edi
	subl	$1, 100(%rsp)
	movw	%cx, 16(%rsp)
	call	create_map
	leaq	128(%rsp), %rsi
	movq	%rbx, %rdi
	movq	%rax, 8(%rsp)
	movq	$0, 128(%rsp)
	call	get_file_contents
	movq	%rax, %r13
	movq	128(%rsp), %rax
	testq	%rax, %rax
	jle	.L82
	movl	$0, 140(%rsp)
	movq	%r13, %rbx
	leaq	1(%r13), %rbp
	movq	%r13, %r12
	xorl	%r15d, %r15d
	jmp	.L86
	.p2align 4,,10
	.p2align 3
.L83:
	cmpb	$10, %dl
	movq	%r12, %rcx
	je	.L102
.L84:
	subq	$1, %rax
	addq	$1, %rbx
	addq	$1, %rbp
	testq	%rax, %rax
	movq	%rax, 128(%rsp)
	jle	.L82
	movq	%rcx, %r12
.L86:
	movzbl	(%rbx), %edx
	movq	%rbx, %rcx
	cmpb	$44, %dl
	jne	.L83
	movl	%ebx, %r15d
	subl	%r13d, %r15d
	jmp	.L84
	.p2align 4,,10
	.p2align 3
.L69:
	testl	%ebx, %ebx
	je	.L70
	movslq	8(%rsp), %rax
	movl	%ebp, %edx
	addl	$1, 8(%rsp)
	subl	%r14d, %edx
	xorl	%ebx, %ebx
	salq	$5, %rax
	addq	16(%rsp), %rax
	movq	%r14, 16(%rax)
	movl	%edx, 24(%rax)
	leaq	1(%rbp), %r14
	jmp	.L67
	.p2align 4,,10
	.p2align 3
.L101:
	movslq	8(%rsp), %r15
	leaq	1(%rbp), %r14
	movl	$10, %edx
	xorl	%esi, %esi
	movq	%r14, %rdi
	salq	$5, %r15
	addq	16(%rsp), %r15
	movq	$0, 16(%r15)
	call	strtol
	movl	%eax, 24(%r15)
	movl	$1, %r15d
	jmp	.L67
	.p2align 4,,10
	.p2align 3
.L70:
	testl	%r15d, %r15d
	je	.L71
	addl	$1, 8(%rsp)
	leaq	1(%rbp), %r14
	xorl	%r15d, %r15d
	jmp	.L67
	.p2align 4,,10
	.p2align 3
.L75:
	movsbw	(%rcx), %dx
	movswl	%dx, %edx
	jmp	.L74
	.p2align 4,,10
	.p2align 3
.L71:
	movslq	8(%rsp), %rax
	addl	$1, 8(%rsp)
	salq	$5, %rax
	addq	16(%rsp), %rax
	movq	$0, 16(%rax)
	movl	$0, 24(%rax)
	jmp	.L96
	.p2align 4,,10
	.p2align 3
.L82:
	movl	$2, %esi
	movl	$1, %edi
	subq	$16, %r14
	call	calloc
	movzwl	16(%rsp), %edx
	movq	112(%rsp), %rcx
	movq	%rax, %rsi
	movq	8(%rcx), %rdi
	movw	%dx, (%rax)
	movq	8(%rsp), %rdx
	call	rb_tree_insert
	movl	100(%rsp), %eax
	testl	%eax, %eax
	jne	.L87
	movq	112(%rsp), %rax
	addq	$152, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
.L102:
	.cfi_restore_state
	leaq	1(%r12), %rdi
	movl	$10, %edx
	xorl	%esi, %esi
	call	strtol
	movq	8(%rsp), %rdi
	leaq	140(%rsp), %rcx
	movq	%r13, %rsi
	movl	$4, %r8d
	movl	%r15d, %edx
	movl	%eax, 140(%rsp)
	movq	%rbp, %r13
	call	insert_map
	movq	128(%rsp), %rax
	movq	%r12, %rcx
	jmp	.L84
	.cfi_endproc
.LFE47:
	.size	create_translator, .-create_translator
	.section	.rodata.str1.1
.LC3:
	.string	"t destroy called. "
	.text
	.p2align 4,,15
.globl i_info_destroy
	.type	i_info_destroy, @function
i_info_destroy:
.LFB36:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	movq	%rdi, %rbx
	.cfi_offset 3, -16
	movl	$.LC3, %edi
	call	puts
	movq	%rbx, %rdi
	popq	%rbx
	.cfi_def_cfa_offset 8
	jmp	delete_map
	.cfi_endproc
.LFE36:
	.size	i_info_destroy, .-i_info_destroy
	.p2align 4,,15
.globl t_info_destroy
	.type	t_info_destroy, @function
t_info_destroy:
.LFB35:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	movq	%rdi, %rbx
	.cfi_offset 3, -16
	movl	$.LC3, %edi
	call	puts
	movq	%rbx, %rdi
	call	get_stuff
	movq	%rbx, %rdi
	movq	%rax, %rsi
	movq	$0, (%rax)
	call	return_stuff
	movq	%rbx, %rdi
	call	get_stuff
	movq	%rbx, %rdi
	movq	%rax, %rsi
	movq	$0, (%rax)
	call	return_stuff
	movq	%rbx, %rdi
	call	get_stuff
	movq	%rbx, %rdi
	movq	%rax, %rsi
	movq	$0, (%rax)
	call	return_stuff
	movq	%rbx, %rdi
	call	get_stuff
	movq	%rbx, %rdi
	movq	%rax, %rsi
	movq	$0, (%rax)
	call	return_stuff
	movq	%rbx, %rdi
	call	get_stuff
	movq	%rbx, %rdi
	movq	%rax, %rsi
	movq	$0, (%rax)
	call	return_stuff
	movq	%rbx, %rdi
	call	get_stuff
	movq	%rbx, %rdi
	movq	%rax, %rsi
	movq	$0, (%rax)
	call	return_stuff
	movq	%rbx, %rdi
	call	get_stuff
	movq	%rbx, %rdi
	movq	%rax, %rsi
	movq	$0, (%rax)
	call	return_stuff
	movq	%rbx, %rdi
	call	get_stuff
	movq	%rbx, %rdi
	movq	$0, (%rax)
	movq	%rax, %rsi
	call	return_stuff
	movq	%rbx, %rdi
	popq	%rbx
	.cfi_def_cfa_offset 8
	jmp	destroy_mem_glob
	.cfi_endproc
.LFE35:
	.size	t_info_destroy, .-t_info_destroy
	.section	.rodata.str1.1
.LC4:
	.string	"Printing the object"
	.text
	.p2align 4,,15
.globl ft_print_val
	.type	ft_print_val, @function
ft_print_val:
.LFB34:
	.cfi_startproc
	movl	$.LC4, %edi
	xorl	%eax, %eax
	jmp	printf
	.cfi_endproc
.LFE34:
	.size	ft_print_val, .-ft_print_val
	.section	.rodata.str1.1
.LC5:
	.string	"%d"
	.text
	.p2align 4,,15
.globl ft_print
	.type	ft_print, @function
ft_print:
.LFB33:
	.cfi_startproc
	movl	(%rdi), %esi
	xorl	%eax, %eax
	movl	$.LC5, %edi
	jmp	printf
	.cfi_endproc
.LFE33:
	.size	ft_print, .-ft_print
	.section	.rodata.str1.1
.LC6:
	.string	"%03d"
	.text
	.p2align 4,,15
.globl set_check_sum
	.type	set_check_sum, @function
set_check_sum:
.LFB58:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	movq	%rsi, %rbx
	.cfi_offset 3, -16
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	movq	24(%rsi), %rdi
	call	databuf_unread
	movq	24(%rbx), %r10
	xorl	%edx, %edx
	testl	%eax, %eax
	movl	%eax, %r8d
	movq	16(%r10), %rcx
	jle	.L113
	movl	%eax, %r9d
	shrl	$4, %r9d
	movl	%r9d, %edx
	sall	$4, %edx
	cmpl	$15, %eax
	jbe	.L121
	testl	%edx, %edx
	je	.L121
	pxor	%xmm0, %xmm0
	movq	%rcx, %rdi
	pxor	%xmm6, %xmm6
	xorl	%esi, %esi
	pxor	%xmm4, %xmm4
	.p2align 4,,10
	.p2align 3
.L117:
	movdqu	(%rdi), %xmm1
	addl	$1, %esi
	movdqa	%xmm6, %xmm3
	addq	$16, %rdi
	movdqa	%xmm1, %xmm2
	cmpl	%r9d, %esi
	pcmpgtb	%xmm1, %xmm3
	punpcklbw	%xmm3, %xmm2
	punpckhbw	%xmm3, %xmm1
	movdqa	%xmm4, %xmm3
	movdqa	%xmm2, %xmm5
	pcmpgtw	%xmm2, %xmm3
	punpckhwd	%xmm3, %xmm2
	punpcklwd	%xmm3, %xmm5
	movdqa	%xmm1, %xmm3
	paddd	%xmm5, %xmm0
	paddd	%xmm2, %xmm0
	movdqa	%xmm4, %xmm2
	pcmpgtw	%xmm1, %xmm2
	punpcklwd	%xmm2, %xmm3
	punpckhwd	%xmm2, %xmm1
	paddd	%xmm3, %xmm0
	paddd	%xmm1, %xmm0
	jb	.L117
	movdqa	%xmm0, %xmm1
	mov	%edx, %edi
	addq	%rdi, %rcx
	cmpl	%edx, %eax
	psrldq	$8, %xmm1
	paddd	%xmm1, %xmm0
	movdqa	%xmm0, %xmm1
	psrldq	$4, %xmm1
	paddd	%xmm1, %xmm0
	movd	%xmm0, 12(%rsp)
	movl	12(%rsp), %esi
	je	.L118
	.p2align 4,,10
	.p2align 3
.L123:
	movsbl	(%rcx), %eax
	addl	$1, %edx
	addq	$1, %rcx
	addl	%eax, %esi
	cmpl	%edx, %r8d
	jg	.L123
.L118:
	movl	%esi, %eax
	sarl	$31, %eax
	shrl	$24, %eax
	addl	%eax, %esi
	movzbl	%sil, %edx
	subl	%eax, %edx
.L113:
	movq	24(%r10), %rsi
	leaq	1(%rsi), %rcx
	movb	$48, (%rsi)
	movb	$49, 1(%rsi)
	movq	%rcx, %rax
	jmp	.L119
	.p2align 4,,10
	.p2align 3
.L127:
	addq	$1, %rcx
.L119:
	movzbl	(%rsi), %edi
	movzbl	(%rax), %r8d
	movb	%r8b, (%rsi)
	movb	%dil, (%rax)
	subq	$1, %rax
	cmpq	%rax, %rcx
	movq	%rcx, %rsi
	jb	.L127
	movq	24(%rbx), %rax
	movl	$.LC6, %esi
	addq	$2, 24(%rax)
	movq	24(%rax), %rcx
	movb	$61, (%rcx)
	addq	$1, %rcx
	movq	24(%rbx), %rdi
	movq	%rcx, 24(%rax)
	xorl	%eax, %eax
	call	databuf_write
	movq	24(%rbx), %rax
	movq	24(%rax), %rdx
	movb	$1, (%rdx)
	addq	$1, %rdx
	movq	%rdx, 24(%rax)
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
.L121:
	.cfi_restore_state
	xorl	%edx, %edx
	xorl	%esi, %esi
	jmp	.L123
	.cfi_endproc
.LFE58:
	.size	set_check_sum, .-set_check_sum
	.p2align 4,,15
.globl add_time_tag
	.type	add_time_tag, @function
add_time_tag:
.LFB56:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	movl	%ecx, %r12d
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	movq	%rdx, %rbp
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	movq	%rsi, %rbx
	.cfi_offset 3, -32
	subq	$16, %rsp
	.cfi_def_cfa_offset 48
	movq	%rsp, %rdi
	call	ftime
	movq	24(%rbx), %rax
	movslq	%r12d, %rcx
	xorl	%edi, %edi
	movabsq	$-3689348814741910323, %r8
	movq	24(%rax), %rsi
	.p2align 4,,10
	.p2align 3
.L129:
	movq	%rcx, %rax
	mulq	%r8
	shrq	$3, %rdx
	leaq	(%rdx,%rdx,4), %rax
	addq	%rax, %rax
	subq	%rax, %rcx
	movzbl	dig.4143(%rcx), %eax
	movq	%rdx, %rcx
	movb	%al, (%rsi,%rdi)
	addq	$1, %rdi
	testq	%rdx, %rdx
	jne	.L129
	leaq	-1(%rsi,%rdi), %rax
	cmpq	%rax, %rsi
	jae	.L130
	.p2align 4,,10
	.p2align 3
.L133:
	movzbl	(%rsi), %edx
	movzbl	(%rax), %ecx
	movb	%cl, (%rsi)
	movb	%dl, (%rax)
	addq	$1, %rsi
	subq	$1, %rax
	cmpq	%rax, %rsi
	jb	.L133
.L130:
	movq	24(%rbx), %rax
	mov	%edi, %edi
	movl	$21, %ecx
	addq	%rdi, 24(%rax)
	movq	%rbp, %rdi
	movq	24(%rax), %rdx
	movb	$61, (%rdx)
	addq	$1, %rdx
	movq	%rdx, 24(%rax)
	movq	24(%rbx), %rax
	movzwl	8(%rsp), %esi
	movq	24(%rax), %rdx
	call	tm_to_str_millis
	movq	24(%rbx), %rax
	addq	$21, 24(%rax)
	movq	24(%rax), %rdx
	movb	$1, (%rdx)
	addq	$1, %rdx
	movq	%rdx, 24(%rax)
	addq	$16, %rsp
	.cfi_def_cfa_offset 32
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE56:
	.size	add_time_tag, .-add_time_tag
	.p2align 4,,15
.globl set_fix_val
	.type	set_fix_val, @function
set_fix_val:
.LFB52:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	movq	24(%rsi), %rax
	movq	%rsi, %rbx
	.cfi_offset 3, -16
	movslq	%edx, %rdi
	xorl	%r9d, %r9d
	movabsq	$-3689348814741910323, %r10
	movq	24(%rax), %rsi
	.p2align 4,,10
	.p2align 3
.L137:
	movq	%rdi, %rax
	mulq	%r10
	shrq	$3, %rdx
	leaq	(%rdx,%rdx,4), %rax
	addq	%rax, %rax
	subq	%rax, %rdi
	movzbl	dig.4143(%rdi), %eax
	movq	%rdx, %rdi
	movb	%al, (%rsi,%r9)
	addq	$1, %r9
	testq	%rdx, %rdx
	jne	.L137
	leaq	-1(%rsi,%r9), %rax
	cmpq	%rax, %rsi
	jae	.L138
	.p2align 4,,10
	.p2align 3
.L141:
	movzbl	(%rsi), %edx
	movzbl	(%rax), %edi
	movb	%dil, (%rsi)
	movb	%dl, (%rax)
	addq	$1, %rsi
	subq	$1, %rax
	cmpq	%rax, %rsi
	jb	.L141
.L138:
	movq	24(%rbx), %rax
	mov	%r9d, %r9d
	movq	%rcx, %rsi
	addq	%r9, 24(%rax)
	movq	24(%rax), %rdx
	movb	$61, (%rdx)
	addq	$1, %rdx
	movq	24(%rbx), %rdi
	movq	%rdx, 24(%rax)
	movslq	%r8d, %rdx
	call	databuf_memcpy
	movq	24(%rbx), %rax
	movq	24(%rax), %rdx
	movb	$1, (%rdx)
	addq	$1, %rdx
	movq	%rdx, 24(%rax)
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE52:
	.size	set_fix_val, .-set_fix_val
	.p2align 4,,15
.globl add_num_val
	.type	add_num_val, @function
add_num_val:
.LFB60:
	.cfi_startproc
	movq	24(%rsi), %rax
	movslq	%edx, %r8
	xorl	%r9d, %r9d
	movabsq	$-3689348814741910323, %r10
	movq	24(%rax), %rdi
	.p2align 4,,10
	.p2align 3
.L145:
	movq	%r8, %rax
	mulq	%r10
	shrq	$3, %rdx
	leaq	(%rdx,%rdx,4), %rax
	addq	%rax, %rax
	subq	%rax, %r8
	movzbl	dig.4143(%r8), %eax
	movq	%rdx, %r8
	movb	%al, (%rdi,%r9)
	addq	$1, %r9
	testq	%rdx, %rdx
	jne	.L145
	leaq	-1(%rdi,%r9), %rax
	cmpq	%rax, %rdi
	jae	.L146
	.p2align 4,,10
	.p2align 3
.L153:
	movzbl	(%rdi), %edx
	movzbl	(%rax), %r8d
	movb	%r8b, (%rdi)
	movb	%dl, (%rax)
	addq	$1, %rdi
	subq	$1, %rax
	cmpq	%rax, %rdi
	jb	.L153
.L146:
	movq	24(%rsi), %rax
	mov	%r9d, %r9d
	mov	%ecx, %ecx
	xorl	%r8d, %r8d
	addq	%r9, 24(%rax)
	movabsq	$-3689348814741910323, %r9
	movq	24(%rax), %rdx
	movb	$61, (%rdx)
	addq	$1, %rdx
	movq	%rdx, 24(%rax)
	movq	24(%rsi), %rax
	movq	24(%rax), %rdi
	.p2align 4,,10
	.p2align 3
.L148:
	movq	%rcx, %rax
	mulq	%r9
	shrq	$3, %rdx
	leaq	(%rdx,%rdx,4), %rax
	addq	%rax, %rax
	subq	%rax, %rcx
	movzbl	dig.4143(%rcx), %eax
	movq	%rdx, %rcx
	movb	%al, (%rdi,%r8)
	addq	$1, %r8
	testq	%rdx, %rdx
	jne	.L148
	leaq	-1(%rdi,%r8), %rax
	cmpq	%rax, %rdi
	jae	.L149
	.p2align 4,,10
	.p2align 3
.L152:
	movzbl	(%rdi), %edx
	movzbl	(%rax), %ecx
	movb	%cl, (%rdi)
	movb	%dl, (%rax)
	addq	$1, %rdi
	subq	$1, %rax
	cmpq	%rax, %rdi
	jb	.L152
.L149:
	movq	24(%rsi), %rax
	mov	%r8d, %r8d
	addq	%r8, 24(%rax)
	movq	24(%rax), %rdx
	movb	$1, (%rdx)
	addq	$1, %rdx
	movq	%rdx, 24(%rax)
	ret
	.cfi_endproc
.LFE60:
	.size	add_num_val, .-add_num_val
	.p2align 4,,15
.globl overwrite_fix_val
	.type	overwrite_fix_val, @function
overwrite_fix_val:
.LFB53:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	movl	%r9d, %r14d
	.cfi_offset 14, -16
	pushq	%r13
	.cfi_def_cfa_offset 24
	movl	%r8d, %r13d
	.cfi_offset 13, -24
	pushq	%r12
	.cfi_def_cfa_offset 32
	movq	%rcx, %r12
	.cfi_offset 12, -32
	pushq	%rbp
	.cfi_def_cfa_offset 40
	movl	%edx, %ebp
	.cfi_offset 6, -40
	pushq	%rbx
	.cfi_def_cfa_offset 48
	movq	%rsi, %rbx
	.cfi_offset 3, -48
	movswl	%dx, %esi
	movq	(%rbx), %rdi
	call	sti_tree_get_val
	cwtl
	testl	%eax, %eax
	jns	.L169
	movq	24(%rbx), %rax
	movslq	%ebp, %rbp
	xorl	%esi, %esi
	movabsq	$-3689348814741910323, %rdi
	movq	24(%rax), %rcx
	.p2align 4,,10
	.p2align 3
.L162:
	movq	%rbp, %rax
	mulq	%rdi
	shrq	$3, %rdx
	leaq	(%rdx,%rdx,4), %rax
	addq	%rax, %rax
	subq	%rax, %rbp
	movzbl	dig.4143(%rbp), %eax
	movq	%rdx, %rbp
	movb	%al, (%rcx,%rsi)
	addq	$1, %rsi
	testq	%rdx, %rdx
	jne	.L162
	leaq	-1(%rcx,%rsi), %rax
	cmpq	%rax, %rcx
	jae	.L163
	.p2align 4,,10
	.p2align 3
.L166:
	movzbl	(%rcx), %edx
	movzbl	(%rax), %edi
	movb	%dil, (%rcx)
	movb	%dl, (%rax)
	addq	$1, %rcx
	subq	$1, %rax
	cmpq	%rax, %rcx
	jb	.L166
.L163:
	movq	24(%rbx), %rax
	mov	%esi, %esi
	addq	%rsi, 24(%rax)
	movq	%r12, %rsi
	movq	24(%rax), %rdx
	movb	$61, (%rdx)
	addq	$1, %rdx
	movq	24(%rbx), %rdi
	movq	%rdx, 24(%rax)
	movslq	%r13d, %rdx
	call	databuf_memcpy
	movq	24(%rbx), %rax
	movq	24(%rax), %rdx
	movb	$1, (%rdx)
	addq	$1, %rdx
	movq	%rdx, 24(%rax)
	xorl	%eax, %eax
.L161:
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L169:
	.cfi_restore_state
	movslq	%eax, %rdx
	leal	(%r14,%r13), %ecx
	movl	$-1, %eax
	salq	$5, %rdx
	addq	16(%rbx), %rdx
	cmpl	%ecx, 24(%rdx)
	jne	.L161
	movslq	%r14d, %rdi
	addq	16(%rdx), %rdi
	movq	%r12, %rsi
	movslq	%r13d, %rdx
	call	memcpy
	movl	$1, %eax
	jmp	.L161
	.cfi_endproc
.LFE53:
	.size	overwrite_fix_val, .-overwrite_fix_val
	.p2align 4,,15
.globl set_time_tag
	.type	set_time_tag, @function
set_time_tag:
.LFB55:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	movl	%ecx, %r12d
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	movq	%rdx, %rbp
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	movq	%rsi, %rbx
	.cfi_offset 3, -32
	subq	$16, %rsp
	.cfi_def_cfa_offset 48
	movq	%rsp, %rdi
	call	ftime
	movq	(%rbx), %rdi
	movswl	%r12w, %esi
	call	sti_tree_get_val
	cwtl
	testl	%eax, %eax
	jle	.L171
	cltq
	movzwl	8(%rsp), %esi
	movq	%rbp, %rdi
	salq	$5, %rax
	addq	16(%rbx), %rax
	movslq	24(%rax), %rcx
	movq	16(%rax), %rdx
	call	tm_to_str_millis
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L171:
	.cfi_restore_state
	movq	24(%rbx), %rax
	movslq	%r12d, %r12
	xorl	%esi, %esi
	movabsq	$-3689348814741910323, %rdi
	movq	24(%rax), %rcx
	.p2align 4,,10
	.p2align 3
.L173:
	movq	%r12, %rax
	mulq	%rdi
	shrq	$3, %rdx
	leaq	(%rdx,%rdx,4), %rax
	addq	%rax, %rax
	subq	%rax, %r12
	movzbl	dig.4143(%r12), %eax
	movq	%rdx, %r12
	movb	%al, (%rcx,%rsi)
	addq	$1, %rsi
	testq	%rdx, %rdx
	jne	.L173
	leaq	-1(%rcx,%rsi), %rax
	cmpq	%rax, %rcx
	jae	.L174
	.p2align 4,,10
	.p2align 3
.L177:
	movzbl	(%rcx), %edx
	movzbl	(%rax), %edi
	movb	%dil, (%rcx)
	movb	%dl, (%rax)
	addq	$1, %rcx
	subq	$1, %rax
	cmpq	%rax, %rcx
	jb	.L177
.L174:
	movq	24(%rbx), %rax
	mov	%esi, %esi
	movq	%rbp, %rdi
	movl	$21, %ecx
	addq	%rsi, 24(%rax)
	movq	24(%rax), %rdx
	movb	$61, (%rdx)
	addq	$1, %rdx
	movq	%rdx, 24(%rax)
	movq	24(%rbx), %rax
	movzwl	8(%rsp), %esi
	movq	24(%rax), %rdx
	call	tm_to_str_millis
	movq	24(%rbx), %rax
	addq	$21, 24(%rax)
	movq	24(%rax), %rdx
	movb	$1, (%rdx)
	addq	$1, %rdx
	movq	%rdx, 24(%rax)
	addq	$16, %rsp
	.cfi_def_cfa_offset 32
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE55:
	.size	set_time_tag, .-set_time_tag
	.p2align 4,,15
.globl destroy_fixy_message
	.type	destroy_fixy_message, @function
destroy_fixy_message:
.LFB50:
	.cfi_startproc
	movq	%rbx, -32(%rsp)
	movq	%rbp, -24(%rsp)
	movq	%rsi, %rbx
	.cfi_offset 6, -32
	.cfi_offset 3, -40
	movq	%r12, -16(%rsp)
	movq	%r13, -8(%rsp)
	movq	%rdi, %r12
	.cfi_offset 13, -16
	.cfi_offset 12, -24
	subq	$40, %rsp
	.cfi_def_cfa_offset 48
	leaq	36(%rsi), %r13
	movq	(%rdi), %rdi
	movq	%r13, %rsi
	call	rb_tree_get_val
	testq	%rax, %rax
	movq	%rax, %rbp
	je	.L184
	movq	(%r12), %rdi
	movq	%r13, %rsi
	call	rb_tree_get_val
	testq	%rax, %rax
	je	.L182
	movq	24(%rbx), %rdx
	movslq	32(%rbx), %rax
	movl	$9, %esi
	movq	(%rbx), %rdi
	addq	16(%rdx), %rax
	movq	%rax, 24(%rdx)
	call	sti_tree_get_val
	cwtl
	testl	%eax, %eax
	jle	.L183
	cltq
	movl	$48, %esi
	salq	$5, %rax
	addq	16(%rbx), %rax
	movslq	24(%rax), %rdx
	movq	16(%rax), %rdi
	call	memset
.L183:
	movq	(%rbx), %rdi
	movl	$34, %esi
	call	sti_tree_get_val
	cwtl
	testl	%eax, %eax
	jle	.L182
	cltq
	movl	$48, %esi
	salq	$5, %rax
	addq	16(%rbx), %rax
	movslq	24(%rax), %rdx
	movq	16(%rax), %rdi
	call	memset
.L182:
	movq	%rbx, %rsi
	movq	%rbp, %rdi
	movq	8(%rsp), %rbx
	movq	16(%rsp), %rbp
	movq	24(%rsp), %r12
	movq	32(%rsp), %r13
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	jmp	return_stuff
	.p2align 4,,10
	.p2align 3
.L184:
	.cfi_restore_state
	movq	8(%rsp), %rbx
	movq	16(%rsp), %rbp
	movq	24(%rsp), %r12
	movq	32(%rsp), %r13
	addq	$40, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE50:
	.size	destroy_fixy_message, .-destroy_fixy_message
	.p2align 4,,15
.globl set_num_val
	.type	set_num_val, @function
set_num_val:
.LFB59:
	.cfi_startproc
	pushq	%r13
	.cfi_def_cfa_offset 16
	movl	%ecx, %r13d
	.cfi_offset 13, -16
	pushq	%r12
	.cfi_def_cfa_offset 24
	movl	%edx, %r12d
	.cfi_offset 12, -24
	pushq	%rbp
	.cfi_def_cfa_offset 32
	movq	%rdi, %rbp
	.cfi_offset 6, -32
	pushq	%rbx
	.cfi_def_cfa_offset 40
	movq	%rsi, %rbx
	.cfi_offset 3, -40
	movswl	%dx, %esi
	subq	$8, %rsp
	.cfi_def_cfa_offset 48
	movq	(%rbx), %rdi
	call	sti_tree_get_val
	movswl	%ax, %r8d
	testl	%r8d, %r8d
	jle	.L186
	movq	16(%rbp), %rax
	mov	%r13d, %r13d
	xorl	%ecx, %ecx
	movabsq	$-3689348814741910323, %rdi
	movq	8(%rax), %rsi
	.p2align 4,,10
	.p2align 3
.L187:
	movq	%r13, %rax
	mulq	%rdi
	shrq	$3, %rdx
	leaq	(%rdx,%rdx,4), %rax
	addq	%rax, %rax
	subq	%rax, %r13
	movzbl	dig.4336(%r13), %eax
	movq	%rdx, %r13
	movb	%al, (%rsi,%rcx)
	addq	$1, %rcx
	testq	%rdx, %rdx
	jne	.L187
	movslq	%r8d, %r8
	salq	$5, %r8
	addq	16(%rbx), %r8
	movslq	24(%r8), %rax
	subq	%rcx, %rax
	addq	16(%r8), %rax
	leaq	(%rsi,%rcx), %rcx
	.p2align 4,,10
	.p2align 3
.L188:
	movzbl	-1(%rcx), %edx
	subq	$1, %rcx
	movb	%dl, (%rax)
	addq	$1, %rax
	cmpq	%rsi, %rcx
	jne	.L188
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L186:
	.cfi_restore_state
	movq	24(%rbx), %rax
	movslq	%r12d, %r12
	xorl	%esi, %esi
	movabsq	$-3689348814741910323, %rdi
	movq	24(%rax), %rcx
	.p2align 4,,10
	.p2align 3
.L190:
	movq	%r12, %rax
	mulq	%rdi
	shrq	$3, %rdx
	leaq	(%rdx,%rdx,4), %rax
	addq	%rax, %rax
	subq	%rax, %r12
	movzbl	dig.4143(%r12), %eax
	movq	%rdx, %r12
	movb	%al, (%rcx,%rsi)
	addq	$1, %rsi
	testq	%rdx, %rdx
	jne	.L190
	leaq	-1(%rcx,%rsi), %rax
	cmpq	%rax, %rcx
	jae	.L191
	.p2align 4,,10
	.p2align 3
.L198:
	movzbl	(%rcx), %edx
	movzbl	(%rax), %edi
	movb	%dil, (%rcx)
	movb	%dl, (%rax)
	addq	$1, %rcx
	subq	$1, %rax
	cmpq	%rax, %rcx
	jb	.L198
.L191:
	movq	24(%rbx), %rax
	mov	%esi, %esi
	mov	%r13d, %r13d
	movabsq	$-3689348814741910323, %rdi
	addq	%rsi, 24(%rax)
	xorl	%esi, %esi
	movq	24(%rax), %rdx
	movb	$61, (%rdx)
	addq	$1, %rdx
	movq	%rdx, 24(%rax)
	movq	24(%rbx), %rax
	movq	24(%rax), %rcx
	.p2align 4,,10
	.p2align 3
.L193:
	movq	%r13, %rax
	mulq	%rdi
	shrq	$3, %rdx
	leaq	(%rdx,%rdx,4), %rax
	addq	%rax, %rax
	subq	%rax, %r13
	movzbl	dig.4143(%r13), %eax
	movq	%rdx, %r13
	movb	%al, (%rcx,%rsi)
	addq	$1, %rsi
	testq	%rdx, %rdx
	jne	.L193
	leaq	-1(%rcx,%rsi), %rax
	cmpq	%rax, %rcx
	jae	.L194
	.p2align 4,,10
	.p2align 3
.L197:
	movzbl	(%rcx), %edx
	movzbl	(%rax), %edi
	movb	%dil, (%rcx)
	movb	%dl, (%rax)
	addq	$1, %rcx
	subq	$1, %rax
	cmpq	%rax, %rcx
	jb	.L197
.L194:
	movq	24(%rbx), %rax
	mov	%esi, %esi
	addq	%rsi, 24(%rax)
	movq	24(%rax), %rdx
	movb	$1, (%rdx)
	addq	$1, %rdx
	movq	%rdx, 24(%rax)
	addq	$8, %rsp
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE59:
	.size	set_num_val, .-set_num_val
	.section	.rodata
	.align 32
	.type	dig.4143, @object
	.size	dig.4143, 37
dig.4143:
	.string	"0123456789abcdefghijklmnopqrstuvwxyz"
	.align 32
	.type	dig.4336, @object
	.size	dig.4336, 37
dig.4336:
	.string	"0123456789abcdefghijklmnopqrstuvwxyz"
	.align 32
	.type	dig.4216, @object
	.size	dig.4216, 37
dig.4216:
	.string	"0123456789abcdefghijklmnopqrstuvwxyz"
	.ident	"GCC: (GNU) 4.4.4 20100630 (Red Hat 4.4.4-10)"
	.section	.note.GNU-stack,"",@progbits

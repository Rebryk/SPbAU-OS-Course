    .code64
	.global empty, pit, pop_error
	.extern handler
       
pit:
	push %rax
	push %rcx
	push %rsi
	push %rdi
	push %r8
	push %r9
	push %r10
	push %r11

	call handler
	
	pop %r11
	pop %r10
	pop %r9
	pop %r8
	pop %rdi
	pop %rsi
	pop %rcx
	pop %rax
	
	iretq
 
empty:
	iretq

pop_error:
	add $8, %rsp #move stack pointer
	iretq
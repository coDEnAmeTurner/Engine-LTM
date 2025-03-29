.code
switch_to_context proc
	; Store NT_TIB stack info members
	; rsp would be here, then here come the pushes
	push qword ptr gs:[8]
	push qword ptr gs:[16]

	; Store general-purpose registers
	push rbx
	push rbp
	push rdi
	push rsi
	push r12
	push r13
	push r14
	push r15

	; Store XMM registers manually
	movaps [rsp - 24], xmm6
	movaps [rsp - 40], xmm7
	movaps [rsp - 56], xmm8
	movaps [rsp - 72], xmm9
	movaps [rsp - 88], xmm10
	movaps [rsp - 104], xmm11
	movaps [rsp - 120], xmm12
	movaps [rsp - 136], xmm13
	movaps [rsp - 152], xmm14
	movaps [rsp - 168], xmm15

	; Store stack pointer
	mov [rcx], rsp
switch_to_context endp

stack_switch_finish proc
	mov rsp, rdx

	; Restore XMM registers manually
	movaps xmm15, [rsp - 168]
	movaps xmm14, [rsp - 152]
	movaps xmm13, [rsp - 136]
	movaps xmm12, [rsp - 120]
	movaps xmm11, [rsp - 104]
	movaps xmm10, [rsp - 88]
	movaps xmm9, [rsp - 72]
	movaps xmm8, [rsp - 56]
	movaps xmm7, [rsp - 40]
	movaps xmm6, [rsp - 24]

	; Pop general-purpose registers
	pop r15
	pop r14
	pop r13
	pop r12
	pop rsi
	pop rdi
	pop rbp
	pop rbx
	
	; Restore NT_TIB stack info memberss
	pop qword ptr gs:[16]
	pop qword ptr gs:[8]

	ret
stack_switch_finish endp

callable_context_start proc
	mov rcx, rdi
	call rbp
	mov rdx, [rbx]
	jmp stack_switch_finish
callable_context_start endp

end 
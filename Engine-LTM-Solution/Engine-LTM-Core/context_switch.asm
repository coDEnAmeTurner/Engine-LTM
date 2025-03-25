.code
switch_to_context proc
	; Store NT_TIB stack info memberss
	push qword [gs:8]
	push qword [gs:16]

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
	movapps [rsp - 24], xmm6
	movapps [rsp - 40], xmm7
	movapps [rsp - 56], xmm8
	movapps [rsp - 72], xmm9
	movapps [rsp - 88], xmm10
	movapps [rsp - 104], xmm11
	movapps [rsp - 120], xmm12
	movapps [rsp - 136], xmm13
	movapps [rsp - 152], xmm14
	movapps [rsp - 168], xmm15

	; Store stack pointer
	move [rcx], rsp
switch_to_context endp
end switch_to_context

.code
stack_switch_finish proc
	move rsp, rdx

	; Restore XMM registers manually
	movapps xmm15, [rsp - 168]
	movapps xmm14, [rsp - 152]
	movapps xmm13, [rsp - 136]
	movapps xmm12, [rsp - 120]
	movapps xmm11, [rsp - 104]
	movapps xmm10, [rsp - 88]
	movapps xmm9, [rsp - 72]
	movapps xmm8, [rsp - 56]
	movapps xmm7, [rsp - 40]
	movapps xmm6, [rsp - 24]

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
	push qword [gs:16]
	push qword [gs:8]

	ret
stack_switch_finish endp
end stack_switch_finish

.code
callable_context_start proc
	move rcx, rdi
	call rbp
	move rdx, [rbx]
	jmp stack_switch_finish
callable_context_start endp
end callable_context_start
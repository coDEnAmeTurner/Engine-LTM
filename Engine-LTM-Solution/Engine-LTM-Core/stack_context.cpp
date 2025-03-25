#include "stack_context.h"

extern "C" void switch_to_context(void** old_stack_top, const void* new_stack_top);
extern "C" void callable_context_start();

void stack_context::switch_into() {
	switch_to_context(&caller_stack_top, my_stack_top);
}
void stack_context::switch_out_of() {
	switch_to_context(&my_stack_top, caller_stack_top);
}
static void* ensure_alignment(void* stack, size_t stack_size) {
	static const size_t CONTEXT_STACK_ALIGNMENT = 16;
	unsigned char* stack_top = static_cast<unsigned char*>(stack) + stack_size;
	return stack_top - reinterpret_cast<size_t>(stack_top) % CONTEXT_STACK_ALIGNMENT;
}
stack_context::stack_context(void* stack, size_t stack_size, void(*function)(void*), void* arg) 
	: caller_stack_top(nullptr), my_stack_top(nullptr) 
{
	unsigned char* math_stack = static_cast<unsigned char*>(ensure_alignment(stack, stack_size));
    my_stack_top = math_stack - sizeof(void*) // space for return address (initial call)
        - sizeof(void*) * 2 // space for stack info
        - sizeof(void*) * 4 // space for arguments
        - sizeof(void*) * 8 // space for non-volatile integer registers
        //- sizeof(void *) * 2 * 10 // space for non-volatile xmm registers
        //- sizeof(void *) // stack alignment
        ;
    void** initial_stack = static_cast<void**>(my_stack_top);
    // initial_stack[11] to initial_stack[14] are space for arguments. I won't
    // use that space but the calling convention says it has to be there
    initial_stack[10] = &callable_context_start;
    initial_stack[9] = math_stack;
    initial_stack[8] = stack;
    initial_stack[7] = &caller_stack_top; // initial rbx
    initial_stack[6] = function; // initial rbp
    initial_stack[5] = arg; // initial rdi
    initial_stack[4] = nullptr; // initial rsi
    initial_stack[3] = nullptr; // initial r12
    initial_stack[2] = nullptr; // initial r13
    initial_stack[1] = nullptr; // initial r14
    initial_stack[0] = nullptr; // initial r15
    initial_stack[-1] = nullptr; // stack alignment
    initial_stack[-3] = initial_stack[-2] = nullptr; // initial xmm6
    initial_stack[-5] = initial_stack[-4] = nullptr; // initial xmm7
    initial_stack[-7] = initial_stack[-6] = nullptr; // initial xmm8
    initial_stack[-9] = initial_stack[-8] = nullptr; // initial xmm9
    initial_stack[-11] = initial_stack[-10] = nullptr; // initial xmm10
    initial_stack[-13] = initial_stack[-12] = nullptr; // initial xmm11
    initial_stack[-15] = initial_stack[-14] = nullptr; // initial xmm12
    initial_stack[-17] = initial_stack[-16] = nullptr; // initial xmm13
    initial_stack[-19] = initial_stack[-18] = nullptr; // initial xmm14
    initial_stack[-21] = initial_stack[-20] = nullptr; // initial xmm15
}
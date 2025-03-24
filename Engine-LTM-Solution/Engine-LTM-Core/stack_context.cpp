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
}
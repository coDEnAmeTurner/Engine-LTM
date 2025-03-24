#pragma once
struct stack_context
{
	stack_context(void* stack, size_t stack_size, void(*function)(void*), void* argument);
	void switch_into();
	void switch_out_of();

private:
	void* caller_stack_top;
	void* my_stack_top;
#ifndef _WIN64	
	void* rbp_on_stack;
#endif

	stack_context(const stack_context&);
	stack_context& operator=(const stack_context&);
	stack_context(stack_context&&);
	stack_context& operator=(stack_context&&);
};



#ifdef TEST_THREAD
#include <iostream>
#include <pthread.h>
#include <Windows.h>

struct multiple_params {
	std::uint32_t ui32;
	std::int32_t i32;
	std::float_t f32;
	std::double_t f64;
};
void* print_new_thread(void* v) {
	void* return_val = nullptr;
	multiple_params* ptr = reinterpret_cast<multiple_params*>(v);
	std::cout << "New thread: "
		<< (*ptr).f32
		<< "\t"
		<< (*ptr).f64
		<< "\t"
		<< (*ptr).i32
		<< "\t"
		<< (*ptr).ui32
		<< "\n";
	pthread_exit(return_val);
	std::cout << "About to exit naturally!\n";
	return return_val;
}
void test_parallel_concurrency() {
	pthread_t ptr;
	multiple_params passed;
	passed.f32 = 32.1256f;
	passed.f64 = 32.1256;
	passed.i32 = 32;
	passed.ui32 = 32U;
	void* v = &passed;
	pthread_create(&ptr, nullptr, print_new_thread, v);
}

#endif
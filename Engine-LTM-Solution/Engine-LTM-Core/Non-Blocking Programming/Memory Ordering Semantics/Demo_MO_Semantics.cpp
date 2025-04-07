#include <thread>
#include <atomic>
#include <assert.h>
#include <iostream>
#include <Windows.h>
#include "BasicSpinLock/BasicSpinLock.h"
#include "Lock-Not-Needed Assertion/UnnecessaryLock.h"

std::atomic<std::uint32_t> g_data = 0;
std::atomic<std::uint32_t> g_ready = 0;

#pragma region Producer Consumer with MO Semantics, assuming atomicity is guaranteed
void setThreadAffinity(int core_id) {
	DWORD_PTR mask = 1ULL << core_id; // shift 1 in 64 bits, to the left, core_id bit place
	HANDLE handle = GetCurrentThread();

	DWORD_PTR result = SetThreadAffinityMask(handle, mask);
	if (result == 0) {
		std::cerr << "Error setting thread affinity: " << GetLastError() << "\n";
	}
}

void ProducerThread() {
	setThreadAffinity(0); // Set thread 1 to core 0

	g_data = 42;

	//std::atomic_thread_fence(std::memory_order_release);

	//the release fence is placed somewhere here
	g_ready.store(1, std::memory_order_release); // Notify the consumer that data is ready
}

void ConsumerThread() {
	setThreadAffinity(1); // Set thread 1 to core 0

	while (!g_ready.load(std::memory_order_acquire)) {
		_mm_pause(); // Spin-wait	
	}
	//the acquire fence is placed somewhere here

	//std::atomic_thread_fence(std::memory_order_acquire);

	std::cout << "Data: " << g_data << std::endl; // Should print 42
}
#pragma endregion
#pragma region BasicSpinLock
inline extern void ProducerThread_Basic();
inline extern void ConsumerThread_Basic();
#pragma endregion
#pragma region ReentrantSpinLock
inline extern void ProducerThread_reen();
inline extern void ConsumerThread_reen();
#pragma endregion
#pragma region Lock not needed assertions
inline extern void ThreadFunction_ul();
inline extern void consume_from_LF_linked_list();
inline extern void produce_into_LF_linked_list();
#pragma endregion


void Demo_MO_Semantics() {

	std::thread t1(produce_into_LF_linked_list);
	std::thread t2(consume_from_LF_linked_list);

	std::cout << "Thread 1: " << t1.get_id() << std::endl;
	std::cout << "Thread 2: " << t2.get_id() << std::endl;

	t1.join();
	t2.join();

}

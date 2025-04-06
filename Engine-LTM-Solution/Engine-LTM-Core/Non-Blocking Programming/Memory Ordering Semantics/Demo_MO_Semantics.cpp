#include <thread>
#include <atomic>
#include <assert.h>
#include <iostream>
#include <Windows.h>
#include "BasicSpinLock/BasicSpinLock.h"

std::atomic<std::uint32_t> g_data = 0;
std::uint32_t g_data_na = 0;
std::atomic<std::uint32_t> g_ready = 0;
std::uint32_t g_ready_na = 0;
BasicSpinLock basic_spinlock;

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
void ProducerThread_Basic() {
	//setThreadAffinity(0); // Set thread 1 to core 0

	basic_spinlock.Acquire();
	g_data_na = 42;
	g_ready_na = true;
	basic_spinlock.Release();
}

void ConsumerThread_Basic() {
	//setThreadAffinity(1); // Set thread 1 to core 1

	basic_spinlock.Acquire();
	while (!g_ready_na){
		basic_spinlock.Release();

	}
	basic_spinlock.Release();
	basic_spinlock.Acquire();
	std::cout << "Data: " << g_data_na << std::endl; // Should print 42
	basic_spinlock.Release();
}
#pragma endregion


void Demo_MO_Semantics() {

	std::thread t1(ProducerThread_Basic);
	std::thread t2(ConsumerThread_Basic);

	std::cout << "Thread 1: " << t1.get_id() << std::endl;
	std::cout << "Thread 2: " <<  t2.get_id() << std::endl;

	t1.join();
	t2.join();

}

#include <thread>
#include <atomic>
#include <assert.h>
#include <iostream>
#include <Windows.h>

std::atomic<std::uint32_t> g_data = 0;
std::atomic<std::uint32_t> g_ready = 0;

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

	g_data.store(42, std::memory_order_relaxed);

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

	std::cout << "Data: " << g_data.load(std::memory_order_relaxed) << std::endl; // Should print 42
}

void Demo_MO_Semantics() {

	std::thread t1(ProducerThread);
	std::thread t2(ConsumerThread);


	t1.join();
	t2.join();

}

#ifdef THREAD_SYNCHRONIZATION_PRIMITIVES
#include <mutex>
#include <iostream>
#include <thread>
#include <Windows.h>

int g_count = 0;
std::mutex	g_mutex;
CRITICAL_SECTION g_critsec;

#pragma region Critical Section and Mutex
inline void IncrementCountT1() {
	//g_mutex.lock();
	EnterCriticalSection(&g_critsec);

	int lo_count = g_count;
	//this is to simulate it blocks because of some blocking system call
	std::this_thread::sleep_for(std::chrono::seconds(2));
	lo_count = lo_count + 1;
	g_count = lo_count;

	LeaveCriticalSection(&g_critsec);

	//g_mutex.unlock();

	return;
}
inline void IncrementCountT2() {
	//g_mutex.lock();
	EnterCriticalSection(&g_critsec);

	int lo_count = g_count;
	std::this_thread::sleep_for(std::chrono::seconds(2));
	lo_count = lo_count + 1;
	g_count = lo_count;

	LeaveCriticalSection(&g_critsec);
	//g_mutex.unlock();

	return;
}
void setThreadAffinity(std::thread& t, int core_id) {
	DWORD_PTR mask = 1ULL << core_id; // shift 1 in 64 bits, to the left, core_id bit place
	HANDLE handle = static_cast<HANDLE>(t.native_handle());

	DWORD_PTR result = SetThreadAffinityMask(handle, mask);
	if (result == 0) {
		std::cerr << "Error setting thread affinity: " << GetLastError() << "\n";
	}
}
#pragma endregion

void Demo_CriticalSection_And_Mutex() {
	InitializeCriticalSection(&g_critsec);

	std::cout << "g_count before incremented: " << g_count << "\n";

	std::thread t1(IncrementCountT1);
	std::thread t2(IncrementCountT2);

	setThreadAffinity(t1, 0);
	setThreadAffinity(t2, 0);

	t1.join();
	t2.join();

	DeleteCriticalSection(&g_critsec);
	std::cout << "g_count after incremented: " << g_count << "\n";
}

void test_parallel_concurrency() {
	Demo_CriticalSection_And_Mutex();


}
#endif
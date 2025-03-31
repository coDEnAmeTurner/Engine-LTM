#ifdef THREAD_SYNCHRONIZATION_PRIMITIVES
#include <mutex>
#include <iostream>
#include <thread>
#include <Windows.h>
#include <queue>

int g_count = 0;
int g_count_cv = 0;
std::queue<int> g_queue;
std::mutex	g_mutex;
bool g_ready = false;
bool g_stop = false;
int result = 0;
std::condition_variable g_cv;
CRITICAL_SECTION g_critsec;

void ProduceDataInto(std::queue<int>& g_queue) {
	g_queue.push(g_count_cv++);
	std::cout << "[Producer] Produced: " << g_count_cv - 1 << "\n";
}
void ConsumeDataFrom(std::queue<int>& g_queue) {
	if (!g_queue.empty())
	{
		int data = g_queue.front();
		g_queue.pop();
		std::cout << "[Consumer] Consumed: " << data << "\n";
	}
}

#pragma region Critical Section and Mutex
inline void IncrementCountT1() {
	//g_mutex.lock();
	EnterCriticalSection(&g_critsec);

	int lo_count = g_count;
	//this is to simulate it blocks because of some blocking system call
	std::this_thread::sleep_for(std::chrono::seconds(1));
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
#pragma region Condition Variables
void ProducerThread_mutex() {
	while (true) {

		if (g_count_cv <= 5)
		{
			g_mutex.lock();
			ProduceDataInto(g_queue);
			g_mutex.unlock();
		}

		g_mutex.lock();
		g_ready = true;
		g_mutex.unlock();


		if (g_count_cv > 5)
		{
			g_mutex.lock();
			g_stop = true;
			g_mutex.unlock();
			return;
		}
		g_count_cv++;

		std::this_thread::yield();
	}

}
void ConsumerThread_mutex() {
	while (true) {
		//loop (wait) till the queue is ready to be consumed
		while (true) {
			g_mutex.lock();
			const bool ready = g_ready;
			g_mutex.unlock();
			if (ready)
			{
				break;
			}

		}

		//wait finished, now consume!!!
		g_mutex.lock();
		if (g_stop)
		{
			g_ready = false;
			g_mutex.unlock();

			return;
		}
		g_mutex.unlock();

		g_mutex.lock();
		ConsumeDataFrom(g_queue);
		g_mutex.unlock();

		g_mutex.lock();
		g_ready = false;
		g_mutex.unlock();

		std::this_thread::yield();

	}

}
void ProducerThread_cv() {
	while (true) {
		std::unique_lock<std::mutex> lock(g_mutex);
		ProduceDataInto(g_queue);
		g_ready = true;
		g_cv.notify_one();
	}

}
void ConsumerThread_cv() {
	while (true) {
		std::unique_lock<std::mutex> lock(g_mutex);
		g_cv.wait(lock, []() {return g_ready; });
		ConsumeDataFrom(g_queue);
		g_ready = false;
	}
}
#pragma endregion


#pragma region Driver
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
void Demo_Condition_Variables_mutex() {
	std::cout << "Result before: " << result << "\n";

	std::thread t1(ProducerThread_mutex);
	std::thread t2(ConsumerThread_mutex);

	setThreadAffinity(t1, 1);
	setThreadAffinity(t2, 1);

	t1.join();
	t2.join();

	std::cout << "Result after: " << result << "\n";

}
void Demo_Condition_Variables_cv() {
	std::thread t1(ProducerThread_cv);
	std::thread t2(ConsumerThread_cv);

	setThreadAffinity(t1, 1);
	setThreadAffinity(t2, 1);

	t1.join();
	t2.join();

}
#pragma endregion

void test_parallel_concurrency() {
	//Demo_CriticalSection_And_Mutex();

	//Demo_Condition_Variables_mutex();

	Demo_Condition_Variables_cv();
}
#endif
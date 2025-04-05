#include <intrin.h>
#include <thread>
#include <chrono>
#include <iostream>
#include "../Memory Ordering Semantics/ReentrantLock32/ReentrantLock32.h"

int count_tas = 0;
bool spinlock_tas = 0b00000000;
ReentrantLock32 lock;

void SpinLockTAS(bool* plock) {
	long* plocklong = reinterpret_cast<long*>(plock);
	while (_interlockedbittestandset(plocklong, 0) == true) {
		_mm_pause();
	}
}

void IncrementCount_TAS() {
	//SpinLockTAS(&spinlock_tas);
	lock.Acquire();

	int count = count_tas;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	count = count + 1;
	count_tas = count;

	lock.Release();
	//spinlock_tas = 0b00000000;
}

void Demo_SpinLockTAS() {
	std::cout << "Count before being incremented: " << count_tas << std::endl;

	std::thread t1(IncrementCount_TAS);
	std::thread t2(IncrementCount_TAS);

	t1.join();
	t2.join();

	std::cout << "Count after being incremented: " << count_tas << std::endl;
}
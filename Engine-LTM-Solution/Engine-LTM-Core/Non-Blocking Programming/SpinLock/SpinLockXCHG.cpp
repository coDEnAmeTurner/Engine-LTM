#include <intrin.h>
#include <thread>
#include <iostream>

bool spinlock_xchg = 0b00000000;
int count_xchg = 0;

void SpinLockXCHG(bool* plock, bool* old) {
	char* plocklong = reinterpret_cast<char*>(plock);
	while (_InterlockedExchange8(
		plocklong, 
		*old
		) == true) {
		_mm_pause();
	}
}

void IncrementCount_XCHG() {
	bool old = 0b00000001;
	SpinLockXCHG(&spinlock_xchg, &old);

	int count = count_xchg;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	count = count + 1;
	count_xchg = count;

	old = 0b00000000;
	SpinLockXCHG(&spinlock_xchg, &old);
}

void Demo_SpinLockXCHG() {
	std::cout << "Count before being incremented: " << count_xchg << std::endl;

	std::thread t1(IncrementCount_XCHG);
	std::thread t2(IncrementCount_XCHG);

	t1.join();
	t2.join();

	std::cout << "Count after being incremented: " << count_xchg << std::endl;
}
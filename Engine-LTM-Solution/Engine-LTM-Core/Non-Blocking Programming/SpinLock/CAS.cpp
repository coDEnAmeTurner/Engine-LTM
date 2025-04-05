#include <thread>
#include <intrin.h>
#include <iostream>

int count_cas = 0;
bool spinlock_cas = 0b00000000;

void SpinLockCAS(bool* plock, bool* exchange) {
	char* plockchar = reinterpret_cast<char*>(plock);
	while (_InterlockedCompareExchange8(plockchar, *reinterpret_cast<char*>(exchange), *plockchar) == 0b00000001)
	{
		_mm_pause();
	}
}

void IncrementCount_CAS() {
	bool exchange = 0b00000001;
	SpinLockCAS(&spinlock_cas, &exchange);

	int count = count_cas;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	count = count + 1;
	count_cas = count;

	exchange = 0b00000000;
	SpinLockCAS(&spinlock_cas, &exchange);
}

void AtomicIncrementCount_CAS() {
	int oldValue;
	int newValue;
	do {
		oldValue = count_cas;

		_ReadWriteBarrier();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		newValue = oldValue + 1;

	} while (_InterlockedCompareExchange(
		reinterpret_cast<long*>(&count_cas),
		static_cast<long>(newValue),
		static_cast<long>(oldValue)
	) == false);
}

void Demo_SpinLockCAS() {
	std::cout << "Count before being incremented: " << count_cas << std::endl;

	/*std::thread t1(IncrementCount_CAS);
	std::thread t2(IncrementCount_CAS);*/

	std::thread t1(AtomicIncrementCount_CAS);
	std::thread t2(AtomicIncrementCount_CAS);

	t1.join();
	t2.join();

	std::cout << "Count after being incremented: " << count_cas << std::endl;
}
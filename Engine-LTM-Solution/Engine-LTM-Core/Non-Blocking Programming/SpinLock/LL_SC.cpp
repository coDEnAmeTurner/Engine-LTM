#include <thread>
#include <atomic>
#include <iostream>

std::atomic<int> count_llsc = 0;

void AtomicIncrementCount_LLSC() {
	int oldValue;
	int newValue;
	do {
		oldValue = count_llsc.load();
		_ReadWriteBarrier();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		newValue = oldValue + 1;
	} while (count_llsc.compare_exchange_weak(oldValue, newValue) == false);

}

void Demo_SpinLock_LLSC() {
	std::cout << "Count before being incremented: " << count_llsc << std::endl;

	/*std::thread t1(IncrementCount_CAS);
	std::thread t2(IncrementCount_CAS);*/

	std::thread t1(AtomicIncrementCount_LLSC);
	std::thread t2(AtomicIncrementCount_LLSC);

	t1.join();
	t2.join();

	std::cout << "Count after being incremented: " << count_llsc << std::endl;
}
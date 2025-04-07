#include "BasicSpinLock.h"
#include <iostream>

void BasicSpinLock::Acquire() {
	while (TryAcquire()) {
		_mm_pause();
	}

}

bool BasicSpinLock::TryAcquire() {
	bool lock = m_atomic.test_and_set(std::memory_order_acquire);

	return lock;
}

void BasicSpinLock::Release() {
	m_atomic.clear(std::memory_order_release);
}

std::uint32_t g_data_na = 0;
std::uint32_t g_ready_na = 0;
BasicSpinLock basic_spinlock;


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
	while (!g_ready_na) {
		basic_spinlock.Release();

	}
	basic_spinlock.Release();
	basic_spinlock.Acquire();
	std::cout << "Data: " << g_data_na << std::endl; // Should print 42
	basic_spinlock.Release();
}
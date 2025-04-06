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
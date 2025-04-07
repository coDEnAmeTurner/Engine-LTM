#pragma once
#include <atomic>

class BasicSpinLock {
	std::atomic_flag m_atomic = ATOMIC_FLAG_INIT;

public:
	bool TryAcquire();
	void Acquire();
	void Release();
};
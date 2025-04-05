#pragma once
#include <atomic>

class ReentrantLock32 {
	std::atomic<std::size_t> m_atomic;
	std::int32_t m_refcount;
public:
	ReentrantLock32() : m_atomic(0), m_refcount(0) {}

	void Acquire();

	void Release();

	bool TryAcquire();


};
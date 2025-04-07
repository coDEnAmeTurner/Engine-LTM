#include "ReentrantLock32.h"
#include <thread>
#include <iostream>
#include <assert.h>
#include "../ScopedLock/ScopedLock.h"

thread_local int ReentrantLock32::m_refcount = 0;

void ReentrantLock32::Acquire() {
	std::hash<std::thread::id> hasher;
	std::size_t tid = hasher(std::this_thread::get_id());

	if (m_atomic.load(std::memory_order_acquire) != tid) {
		std::size_t unlockValue = 0;
		while (!m_atomic.compare_exchange_weak(
			unlockValue,
			tid,
			std::memory_order_relaxed,
			std::memory_order_relaxed
		)) {
			unlockValue = 0;
			_mm_pause(); // Yield the CPU to other threads;
		}
	}

	++m_refcount;

	std::atomic_thread_fence(std::memory_order_acquire);
}

void ReentrantLock32::Release() {
	std::atomic_thread_fence(std::memory_order_release);

	std::hash<std::thread::id> hasher;
	std::size_t tid = hasher(std::this_thread::get_id());
	std::size_t actual = m_atomic.load(std::memory_order_relaxed);
	assert(actual == tid);

	--m_refcount;

	if (m_refcount == 0)
	{
		m_atomic.store(0, std::memory_order_relaxed);
	}

}

bool ReentrantLock32::TryAcquire() {
	std::hash<std::thread::id> hasher;
	std::size_t tid = hasher(std::this_thread::get_id());

	bool acquired = false;

	if (m_atomic.load(std::memory_order_relaxed) != tid) {
		acquired = true;
	}
	else
	{
		std::size_t unlockValue = 0;
		acquired = m_atomic.compare_exchange_strong(
			unlockValue,
			tid,
			std::memory_order_relaxed,
			std::memory_order_relaxed
		);
	}

	if (acquired)
	{
		++m_refcount;
		std::atomic_thread_fence(std::memory_order_acquire);
	}

	return acquired;
}

std::uint32_t g_data_re = 0;
std::uint32_t g_ready_re = 0;
ReentrantLock32 reen_spinlock;

void ProducerThread_reen() {
	//setThreadAffinity(0); // Set thread 1 to core 0

	ScopedLock<ReentrantLock32> lock(&reen_spinlock);
	g_data_re = 42;
	g_ready_re = true;
}

void ConsumerThread_reen() {
	//setThreadAffinity(1); // Set thread 1 to core 1
	bool first_it = true;
	reen_spinlock.Acquire();
	while (!g_ready_re) {
		if (first_it)
		{
			first_it = false;
			reen_spinlock.Release();
		}

	}
	if (first_it)
		reen_spinlock.Release();

	ScopedLock<ReentrantLock32> lock(&reen_spinlock);
	std::cout << "Data: " << g_data_re << std::endl; // Should print 42
}
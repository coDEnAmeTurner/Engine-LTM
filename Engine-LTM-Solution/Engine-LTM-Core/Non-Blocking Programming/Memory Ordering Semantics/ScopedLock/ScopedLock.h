#pragma once
template<class LOCK>
class ScopedLock
{
	typedef LOCK lock_t;
	lock_t* m_lock;

public:
	explicit ScopedLock(lock_t* lock)
		: m_lock(lock)
	{
		m_lock->Acquire();
	}

	~ScopedLock()
	{
		m_lock->Release();
	}
};
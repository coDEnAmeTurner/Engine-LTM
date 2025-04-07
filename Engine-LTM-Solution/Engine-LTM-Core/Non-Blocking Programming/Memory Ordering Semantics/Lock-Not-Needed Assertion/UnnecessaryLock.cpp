#include "UnnecessaryLock.h"

#ifdef ASSERTIONS_ENABLED
#define BEGIN_ASSERT_LOCK_NOT_NECESSARY(L) (L)->Acquire()
#define END_ASSERT_LOCK_NOT_NECESSARY(L) (L)->Release()
#else
#define BEGIN_ASSERT_LOCK_NOT_NECESSARY(L)
#define END_ASSERT_LOCK_NOT_NECESSARY(L)
#endif

void UnnecessaryLock::Acquire()
{
	assert(!m_locked);

	m_locked = true;
}

void UnnecessaryLock::Release()
{
	assert(m_locked);

	m_locked = false;
}


void DummyClass::QueryData()
{
	 BEGIN_ASSERT_LOCK_NOT_NECESSARY(&m_lock);

	 printf("Do query!!!\n");

	 END_ASSERT_LOCK_NOT_NECESSARY(&m_lock);

	// The lock is not needed here, so we can safely remove it.
	// This is a simple example of how to use the lock assertion macros.
}

DummyClass g_dummyObject;

void ThreadFunction_ul()
{
	// This thread will call the QueryData function.
	g_dummyObject.QueryData();
}

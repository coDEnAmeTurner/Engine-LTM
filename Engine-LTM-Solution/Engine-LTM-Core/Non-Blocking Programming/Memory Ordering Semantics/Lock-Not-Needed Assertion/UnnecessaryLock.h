#pragma once
#include <cassert>
#include <cstdio>
#include <thread>

class UnnecessaryLock
{
	volatile bool m_locked = false;

public:
	void Acquire();
	void Release();	
};

class DummyClass
{
	UnnecessaryLock m_lock;
public:
	void QueryData();
};

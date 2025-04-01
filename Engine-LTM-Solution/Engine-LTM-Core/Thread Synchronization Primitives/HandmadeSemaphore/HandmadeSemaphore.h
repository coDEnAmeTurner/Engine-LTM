#pragma once
#include <mutex>
#include <condition_variable>

class HandmadeSemaphore {
private :
	int m_count;
	int m_count_max;
	std::mutex m_mutex;
	std::condition_variable m_cv;
public:
	explicit HandmadeSemaphore(int initialCount, int startCount = -1)
	{
		m_count_max = initialCount;
		m_count = startCount == -1 ? m_count_max : startCount;
	}

	void Take();
	void Give();
};

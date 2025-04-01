#include "HandmadeSemaphore.h"

void HandmadeSemaphore::Take() {
	std::unique_lock<std::mutex> lock(m_mutex);
	m_cv.wait(lock, [&]() {return m_count > 0; });
	--m_count;
}
void HandmadeSemaphore::Give() {
	std::unique_lock<std::mutex> lock(m_mutex);
	if (m_count != m_count_max)
		++m_count;

	if (m_count == 1)
		m_cv.notify_all();
}
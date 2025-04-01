#include <iostream>
#include <queue>
#include <semaphore>
#include <mutex>
#include "HandmadeSemaphore/HandmadeSemaphore.h"

int g_count_sem = 0;
bool g_stop_sem = false;

std::queue<int> g_queue_sem;
std::counting_semaphore<1> g_semUsed(0);
std::counting_semaphore<1> g_semFree(1);
HandmadeSemaphore g_semUsed_hm(1);
HandmadeSemaphore g_semFree_hm(1);
std::mutex g_mutex_sem;

void ProduceDataInto_sem(std::queue<int>& g_queue) {
	g_queue.push(g_count_sem++);
	std::cout << "[Producer] Produced: " << g_count_sem - 1 << "\n";
}
void ConsumeDataFrom_sem(std::queue<int>& g_queue) {
	while (!g_queue.empty())
	{
		int data = g_queue.front();
		g_queue.pop();
		std::cout << "[Consumer] Consumed: " << data << "\n";
	}
}

void ProducerThreadSem() {
	while (true) {
		//g_semFree.acquire();
		g_semFree_hm.Take();

		if (g_count_sem != 5)
		{
			std::lock_guard<std::mutex> lock(g_mutex_sem);
			ProduceDataInto_sem(g_queue_sem);
		}
		else 
			g_stop_sem = true;

		//g_semUsed.release();
		g_semUsed_hm.Give();

		if (g_stop_sem == true)
			return;
	}
}

void ConsumerThreadSem() {
	while (true) {
		//g_semUsed.acquire();
		g_semUsed_hm.Take();

		if (!g_stop_sem)
		{
			std::lock_guard<std::mutex> lock(g_mutex_sem);
			ConsumeDataFrom_sem(g_queue_sem);
		}
		//g_semFree.release();

		g_semFree_hm.Give();

		if (g_stop_sem)
			return;
	}
}




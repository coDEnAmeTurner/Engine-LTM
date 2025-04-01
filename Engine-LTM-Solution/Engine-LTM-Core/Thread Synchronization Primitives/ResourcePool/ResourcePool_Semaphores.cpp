#include "ResourcePool.h"

void worker(ResourcePool& pool) {
	Resource* res = pool.acquire();
	res->use();
	pool.release(res);
}

void Demo_ResourcePool_Semaphores(){
	ResourcePool pool(3);

	std::vector<std::thread> threads;

	for (int i = 0; i < 6; i++) {
		threads.emplace_back(worker, std::ref(pool));
	}

	for (auto& t : threads) {
		t.join();
	}

	std::cout << "All work done!!!\n";
	return;
}
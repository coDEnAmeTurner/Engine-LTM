#include "ResourcePool.h"

ResourcePool::ResourcePool(int size) : sem(size) {
	for (int i = 0; i < size; i++) {
		resources.emplace_back(i);
	}
}

Resource* ResourcePool::acquire() {
	sem.acquire();
	std::lock_guard<std::mutex> lock(pool_mutex);
	Resource* res = &resources.back();
	resources.pop_back();
	return res;
}

void ResourcePool::release(Resource* res) {
	std::lock_guard<std::mutex> lock(pool_mutex);
	resources.push_back(*res);
	sem.release();
}
#pragma once
#include <iostream>
#include <thread>
#include <semaphore>
#include <mutex>
#include <chrono>
#include <vector>

struct Resource {
	int id;
	Resource(int id) : id(id) {

	}

	void use() {
		std::cout << "Use resource " << id << " in thread " << std::this_thread::get_id() << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(500)); // to simulate work
	}
};

class ResourcePool {
private:
	std::vector<Resource> resources;
	std::counting_semaphore<3> sem;
	std::mutex pool_mutex;
public:
	ResourcePool(int size);

	Resource* acquire();

	void release(Resource* res);
};
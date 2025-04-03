#include "CentralArbiter.h"

void CentralArbiter::request_forks(int id) {
	std::unique_lock<std::mutex> lock(arbiter_mutex);
	condition.wait(lock, [&]() {return forks_in_use[id] && forks_in_use[(id + 1) % NUM_PHILOSOPHERS]; });
	forks_in_use[id] = true;
	forks_in_use[(id + 1) % NUM_PHILOSOPHERS] = true;
}

void CentralArbiter::release_forks(int id) {
	std::unique_lock<std::mutex> lock(arbiter_mutex);
	forks_in_use[id] = false;
	forks_in_use[(id + 1) % NUM_PHILOSOPHERS] = false;
	condition.notify_all();
}
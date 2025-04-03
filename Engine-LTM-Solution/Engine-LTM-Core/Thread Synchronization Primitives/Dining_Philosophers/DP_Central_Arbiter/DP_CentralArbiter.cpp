#include "CentralArbiter.h"

CentralArbiter arbiter;

void philosopher_ca(int id) {
	while (true) {
		{
			std::lock_guard<std::mutex> lock(cout_mutex);
			std::cout << "Philosopher " << id << " is thinking\n";
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		arbiter.request_forks(id);
		{
			std::lock_guard<std::mutex> lock(cout_mutex);
			std::cout << "Philosopher " << id << " is eating\n";
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		arbiter.release_forks(id);
	}
}

void DS_Central_Arbiter() {
	std::vector<std::thread> philosophers;
	for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
		philosophers.push_back(std::thread(philosopher_ca, i));
	}
	for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
		philosophers[i].join();
	}
}
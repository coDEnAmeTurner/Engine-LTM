#include "../DP_Common.h"
#include "../../HandmadeSemaphore/HandmadeSemaphore.h"

struct DP_Props {
	HandmadeSemaphore g_sem;
	std::mutex cout_mutex;
	std::mutex forks_mutex;
	bool forks_in_use[NUM_PHILOSOPHERS] = { false };

	DP_Props() : g_sem(NUM_PHILOSOPHERS - 1) {}
} props;

void philosopher_nminus1(int id) {
	while (true) {
		{
			std::lock_guard<std::mutex> lock(props.cout_mutex);
			std::cout << "Philosopher " << id << " is thinking\n";
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		props.g_sem.Take();

		{
			std::lock_guard<std::mutex> lock(props.forks_mutex);
			props.forks_in_use[id] = true;
			props.forks_in_use[(id + 1) % NUM_PHILOSOPHERS] = true;
		}

		{
			std::lock_guard<std::mutex> lock(props.cout_mutex);
			std::cout << "Philosopher " << id << " is eating\n";
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		{
			std::lock_guard<std::mutex> lock(props.forks_mutex);
			props.forks_in_use[id] = false;
			props.forks_in_use[(id + 1) % NUM_PHILOSOPHERS] = false;
		}

		props.g_sem.Give();
	}
}

void DS_NMinus1_Philo() {
	std::vector<std::thread> philosophers;
	for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
		philosophers.push_back(std::thread(philosopher_nminus1, i));
	}
	for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
		philosophers[i].join();
	}
}
#include "../DP_Common.h"
#include <thread>
#include <iostream>
#include <vector>	
#include <mutex>
#include <chrono>

std::mutex forks[NUM_PHILOSOPHERS];

void philosopher(int id) {
	int left_fork = id;
	int right_fork = (id + 1) % NUM_PHILOSOPHERS;

	/*if (id == NUM_PHILOSOPHERS - 1)
		std::swap(left_fork, right_fork);	*/

	//keep on running forever
	while (true) {
		{
			std::lock_guard<std::mutex> lock(cout_mutex);
			std::cout << "Philosopher " << id << " is thinking\n";
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		{
			std::lock_guard<std::mutex> lock1(forks[left_fork]);
			{
				std::lock_guard<std::mutex> lock2(forks[right_fork]);
				{
					std::lock_guard<std::mutex> lock(cout_mutex);
					std::cout << "Philosopher " << id << " is eating\n";	
				}

			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
}

void DS_Global_Order() {
	std::vector<std::thread> philosophers;
	for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
		philosophers.push_back(std::thread(philosopher, i));
	}
	for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
		philosophers[i].join();
	}
}
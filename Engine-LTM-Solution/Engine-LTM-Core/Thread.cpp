#ifdef TEST_THREAD
#define _GNU_SOURCE  // Required for CPU_SET macros
#include <iostream>
#include <pthread.h>
#include <Windows.h>
#include <string>
#include <thread>
#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>

struct multiple_params {
	std::uint32_t ui32;
	std::int32_t i32;
	std::float_t f32;
	std::double_t f64;
};

struct thread_params {
	int core_id;
	int policy;
	int priority;
};

struct fiber_params {
	void* next_fiber;
	void* parent_fiber;
	void* parent_fiber_2;
};

std::atomic<bool> command_to_stop = false;
std::atomic<bool> allowedprint = false;
std::uint32_t g_aResult[1000];
void* fiberh = nullptr;
void* threadb_fiber = nullptr;

void* print_new_thread(void* v) {
	void* return_val = nullptr;
	//multiple_params* ptr = reinterpret_cast<multiple_params*>(v);
	/*std::cout << "New thread: "
		<< (*ptr).f32
		<< "\t"
		<< (*ptr).f64
		<< "\t"
		<< (*ptr).i32
		<< "\t"
		<< (*ptr).ui32
		<< "\n";
	pthread_exit(return_val);
	std::cout << "About to exit naturally!\n";*/
	while (!command_to_stop) {
		std::cout << "Hello from thread!\n";
	}
	return return_val;
}

void* Compute(void* arg) {
	std::uint32_t startIndex = *(std::uint32_t*)arg;
	std::uint32_t endIndex = startIndex + 250;
	for (std::uint32_t i = startIndex; i < endIndex; i++) {
		g_aResult[i] = i * i;
	}
	Sleep(3000);
	char thread_name[16];  // Buffer to store the thread name
	pthread_t thread_id = pthread_self();  // Get current thread ID
	if (pthread_getname_np(thread_id, thread_name, sizeof(thread_name)) == 0) {
		std::cout << thread_name << " ";
	}
	else {
		std::cout << "<thread name> ";
	}
	std::cout << "Done sleeping!\n";
	return nullptr;
}

#pragma region Yield to relinquish
void* print_while_allowed(void* arg) {
	int core_id = *(int*)arg;

	pthread_t self = pthread_self();

	//lock_thread_to_cores(self, core_id);

	while (!allowedprint) {
		std::cout << "I am waiting and about to give up my time slice\n";
		std::this_thread::yield();
		std::cout << "Trying to print something else?\n";
	}
	std::cout << "Finally! I am allowed to print\n";
	return nullptr;
}

void* run_if_possible(void* arg) {
	int core_id = *(int*)arg;

	pthread_t self = pthread_self();

	//lock_thread_to_cores(self, core_id);

	while (!command_to_stop)
	{
		std::cout << "I run whenever I have the core\n";
		std::this_thread::yield();
		std::cout << "I am also trying to print something else?\n";
	}
	return nullptr;
}

#pragma endregion
#pragma region Priority and affinity
void lock_thread_to_cores(pthread_t thread, int core_id) {
	cpu_set_t cpuset; //core set that the thread will use
	CPU_ZERO(&cpuset); //init the set
	CPU_SET(core_id, &cpuset); //assing core to set

	int ret = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);

	if (ret != 0) {
		perror("pthread_setaffinity_np");
	}
}
void set_thread_priority(pthread_t thread, int policy, int priority) {
	sched_param sch_params;
	sch_params.sched_priority = priority;

	int ret = pthread_setschedparam(thread, policy, &sch_params);
	if (ret != 0) {
		perror("pthread_setschedparam");
	}
}

void* always_print(void* arg) {
	int core_id = *(int*)arg;

	pthread_t self = pthread_self();

	lock_thread_to_cores(self, core_id);

	set_thread_priority(self, SCHED_FIFO, -15);

	while (true) {
		std::cout << "Trying to print something else?\n";
		std::this_thread::yield();
	}
	return nullptr;
}

void* run_always(void* arg) {
	int core_id = *(int*)arg;

	pthread_t self = pthread_self();

	lock_thread_to_cores(self, core_id);

	set_thread_priority(self, SCHED_FIFO, 15);

	while (true)
	{
		std::cout << "I am also trying to print something else : 15?\n";
	}
	return nullptr;
}

void* run_always_prior_10(void* arg) {
	int core_id = *(int*)arg;

	pthread_t self = pthread_self();

	lock_thread_to_cores(self, core_id);

	set_thread_priority(self, SCHED_FIFO, 10);

	while (true)
	{
		std::cout << "I am also trying to print something else : 10?\n";
	}
	return nullptr;
}
#pragma endregion
#pragma region Fiber for threads, Fiber Migration
void WINAPI FiberFunction(void* param) {
	std::uint32_t* fiber_control_block_add = (std::uint32_t*)GetCurrentFiber();
	std::cout << "Fiber " << fiber_control_block_add << " : Running...\n";

	fiber_params fparams = *(fiber_params*)param;

	SwitchToFiber(fparams.next_fiber);

	std::cout << "Fiber " << fiber_control_block_add << " : Resuming...\n";

	if (!fparams.parent_fiber)
		return;

	SwitchToFiber(fparams.parent_fiber);
	
}
void WINAPI FiberFunction_F(void* param) {
	std::uint32_t* fiber_control_block_add = (std::uint32_t*)GetCurrentFiber();
	std::cout << "Fiber " << fiber_control_block_add << " : Running...\n";

	fiber_params fparams = *(fiber_params*)param;

	SwitchToFiber(fparams.next_fiber);

	std::cout << "Fiber " << fiber_control_block_add << " : Resuming...\n";

	if (!fparams.parent_fiber)
		return;

	SwitchToFiber(fparams.parent_fiber);

	//this can only be reached on the 2nd switch to Fiber F
	//how are you so sure that at this point fiberh is already created?
	//- Because fiberh is guranteed to be created before the switch
	// 
	//How can fiberf, which runs in the context of main thread, 
	//switches to fiberh, which runs in the context of threadb?
	//- Then how can fiberf runs in the context of main thread, to which it was switched by fiberh in threadb? 
	//- Same answer, they are shared, as long as they are in their inactive state and their creations are ensured! 
	//
	//Why can't fiber main switches to fiberh?
	//- Since fiber h is not ensured to be created when it is switched, its creation is on fiber b, 
	//- which runs in different thread
	SwitchToFiber(fiberh);
}
void* threadb_entry_point(void* fiberf) {
	threadb_fiber = ConvertThreadToFiber(nullptr);
	if (!threadb_fiber) {
		std::cerr << "Convert thread b to fiber failed!" << GetLastError() << std::endl;
		return nullptr;
	}

	fiber_params hparams({ fiberf, threadb_fiber });
	fiberh = CreateFiber(0, FiberFunction, &hparams);
	if (!fiberh)
	{
		std::cerr << "Create fiber h failed!" << GetLastError() << std::endl;
		return nullptr;
	}
	std::cout << "Fiber " << fiberh << " : Created...\n";

	SwitchToFiber(fiberh);

	DeleteFiber(fiberh);

	return nullptr;
}
#pragma endregion
#pragma region Coroutines

#pragma endregion


void test_parallel_concurrency() {

	//request to exit:
	/*
	pthread_create(&thread, nullptr, print_new_thread, nullptr);
	for (int i = 0; i < 1000; i++) {
		if (i == 750) {
			command_to_stop = true;
			pthread_join(thread, nullptr);
		}

		std::cout << "Hello from main!\n";
	}*/

	//join threads
	//pthread_t thread[4];
	/*for (int i = 0; i < 4; i++) {
		std::uint32_t* p = new std::uint32_t(i * 250);
		pthread_create(&thread[i], nullptr, Compute, p);
		std::string s("Thread ");
		s += std::to_string(i);
		if (pthread_setname_np(thread[i], s.c_str()) != 0) {
			perror("pthread_setname_np");
		}
	}

	for (int i = 0; i < 4; i++) {
		pthread_join(thread[i], nullptr);
	std::cout << "Do check sum here!!!!!!!! \n";
	}*/

	//yield in the case of relinquish the rest of its time slice
	/*pthread_t thread[2];
	int core_id = 0;
	if (pthread_create(&thread[0], nullptr, print_while_allowed, &core_id) != 0) {
		std::cerr << "error creating thread\n";
	}

	if (pthread_create(&thread[1], nullptr, run_if_possible, &core_id) != 0) {
		std::cerr << "error creating thread\n";
	}
	lock_thread_to_cores(thread[0], core_id);
	lock_thread_to_cores(thread[1], core_id);

	Sleep(3000);

	allowedprint = true;
	pthread_join(thread[0], nullptr);

	std::cout << "main thread is about to sleep\n";
	Sleep(3000);

	command_to_stop = true;
	pthread_join(thread[1], nullptr);*/

	//3 threads compete on 1 core, 1
	// voluntarily yields, 1 has slightly higher priority, but Runnable second to the voluntary one, 
	// 1 has the most priority but Runnable last
	//The last one seems to take more core time
	/*pthread_t threads[3];
	int core_id = 0;	

	if (pthread_create(&threads[0], nullptr, always_print, &core_id) != 0) {
		std::cerr << "error creating thread\n";
	}
	if (pthread_create(&threads[1], nullptr, run_always_prior_10, &core_id) != 0) {
		std::cerr << "error creating thread\n";
	}
	if (pthread_create(&threads[2], nullptr, run_always, &core_id) != 0) {
		std::cerr << "error creating thread\n";
	}

	pthread_join(threads[0], nullptr);
	pthread_join(threads[1], nullptr);
	pthread_join(threads[2], nullptr);*/

	//User fibers for a thread
	/*std::cout << "Main Thread: Converting to Fiber\n";

	// Convert the main thread to a fiber
	void* mainFiber = ConvertThreadToFiber(nullptr);
	if (!mainFiber) {
		std::cerr << "Failed to convert main thread to fiber\n";
	}

	// Create a new fiber
	void* fiber2 = CreateFiber(0, FiberFunction, mainFiber);
	if (!fiber2) {
		std::cerr << "Failed to create fiber\n";
	}

	std::cout << "Main Fiber: Switching to Fiber 2\n";

	// Switch to the new fiber
	SwitchToFiber(fiber2);

	std::cout << "Main Fiber: Back in control\n";

	// Cleanup
	DeleteFiber(fiber2);*/

	//Fiber Migration
	/*void* main = ConvertThreadToFiber(nullptr);
	if (!main)
	{
		std::cerr << "Convert main thread to fiber failed!" << GetLastError() << std::endl;
		return;
	}
	fiber_params gparams({ main, main });
	void* fiberg = CreateFiber(0, FiberFunction, &gparams);
	if (!fiberg)
	{
		std::cerr << "Convert main thread to fiber failed!" << GetLastError() << std::endl;
		return;
	}
	std::cout << "Fiber " << fiberg << " : Created...\n";

	fiber_params fparams({ fiberg, main });
	void* fiberf = CreateFiber(0, FiberFunction_F, &fparams);
	if (!fiberf)
	{
		std::cerr << "Convert main thread to fiber failed!" << GetLastError() << std::endl;
		return;
	}
	std::cout << "Fiber " << fiberf << " : Created...\n";

	SwitchToFiber(fiberf);

	SwitchToFiber(fiberg);

	pthread_t threadb;
	//here is where fiberh is created
	pthread_create(&threadb, nullptr, threadb_entry_point, fiberf);

	//here is the second switch to fiberf
	SwitchToFiber(fiberf);
	pthread_join(threadb, nullptr);

	DeleteFiber(fiberf);
	DeleteFiber(fiberg);
	*/

	//Coroutines
	
}

#endif
#pragma once
#include"../DP_Common.h"
#include<thread>
#include<iostream>
#include<vector>
#include<queue>	

class CentralArbiter
{
public:
	void request_forks(int id);
	void release_forks(int id);	
private:
	std::mutex arbiter_mutex;
	std::condition_variable condition;
	bool forks_in_use[NUM_PHILOSOPHERS] = { false };
};


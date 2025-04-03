#pragma once
#include <mutex>
#include <iostream>
#include <vector>

constexpr int NUM_PHILOSOPHERS = 5;
inline std::mutex cout_mutex;

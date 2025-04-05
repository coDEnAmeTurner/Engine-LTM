#ifdef NON_BLOCKING_PROGRAMMING

#pragma region Demo TAS & Spin Lock
inline extern void Demo_SpinLockTAS();
inline extern void Demo_SpinLockXCHG();
inline extern void Demo_SpinLockCAS();
inline extern void Demo_SpinLock_LLSC();
inline extern void Demo_MO_Semantics();
#pragma endregion

void test_parallel_concurrency() {
	Demo_SpinLockTAS();

	//Demo_SpinLockXCHG();

	//Demo_SpinLockCAS();

	//Demo_SpinLock_LLSC();

	//Demo_MO_Semantics();

}


#endif
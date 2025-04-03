#include <intrin.h>

void SpinLockTAS(bool* plock) {
	long* plocklong = reinterpret_cast<long*>(plock);
	while (_interlockedbittestandset(plocklong, 0) == true) {
		_mm_pause();
	}
}
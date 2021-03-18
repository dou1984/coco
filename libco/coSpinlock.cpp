#include "coSpinlock.h"
#include <thread>
#include <chrono>

namespace ashan
{
	const int max_try_lock_count_no_sleep = 100;
	bool coSpinlock::try_lock()
	{
		_type t = off;
		return flag.compare_exchange_strong(t, on);
	}
	void coSpinlock::lock()
	{
		size_t c = 0;
		while (!try_lock())
			if (++c > max_try_lock_count_no_sleep)
				std::this_thread::sleep_for(std::chrono::microseconds(500));
	}
	void coSpinlock::unlock()
	{
		_type t = on;
		flag.compare_exchange_weak(t, off);		
	}
}
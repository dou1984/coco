#ifndef SPINLOCK_H
#define SPINLOCK_H
#include <atomic>
#include <cstddef>

namespace ashan
{
	class coSpinlock
	{		
		using _type = int;
		const _type on = 1;
		const _type off = 0;
		std::atomic<_type> flag{ off };
	public:
		bool try_lock();
		void lock();
		void unlock();
	};
}
#endif
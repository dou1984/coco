#ifndef CO_TIMER_H
#define CO_TIMER_H
#include "evcpp.h"
#include "awaiter.h"

namespace ashan
{
	class coSleep : public evTimer, public awaiter<size_t, false>
	{
		using base = awaiter<size_t, false>;
		int on_timer();
	public:
		coSleep(ev_tstamp itime);	
		~coSleep();
		coSleep(coSleep&) = delete;
		coSleep(coSleep&&) = delete;				
	};
}
#endif

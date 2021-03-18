/*
 *	Copyright (c) 2019-2020 Zhao YunShan(93850592@qq.com)
 */
#ifndef EVTIMER_H__
#define EVTIMER_H__
#include "Common.h"

namespace ashan
{
	class evTimer
	{
		ev_timer m_data;
		ev_tstamp m_itimer = 1.0;
		ev_tstamp m_rtimer = 0;
		uint32_t m_tag = 0;		
		static void _trigger(struct ev_loop* loop, struct ev_timer* w, int revents);
	protected:
		size_t m_verify = MAGIC_WORD;
	public:
		evTimer();
		virtual ~evTimer();
		int init(ev_tstamp itimer, ev_tstamp rtimer);
		int init(ev_tstamp itimer);
		int start(ev_tstamp itimer, ev_tstamp rtimer);
		int start(ev_tstamp itimer);
		int active();
		int stop();
		int again(ev_tstamp itimer, ev_tstamp rtimer);
		int again();
		bool is_active();
		ev_tstamp remaining();
		virtual int on_timer() { return 0; }
	};
}
#endif

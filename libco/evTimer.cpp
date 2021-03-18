/*
 *	Copyright (c) 2019-2020 Zhao YunShan(93850592@qq.com)
 */
#include "evTimer.h"
#include "evLoop.h"
namespace ashan
{
	void evTimer::_trigger(struct ev_loop* loop, struct ev_timer* w, int revents)
	{
		evTimer* _this = (evTimer*)(w->data);
		if (_this != NULL)
		{
			assert(_this->m_verify == MAGIC_WORD);			
			_this->on_timer();
		}
	}
	evTimer::evTimer()
	{
		m_data.data = this;
		ev_timer_init(&m_data, evTimer::_trigger, m_itimer, m_rtimer);
		m_tag = evLoop::tag();
	}
	evTimer::~evTimer()
	{
		assert(m_verify == MAGIC_WORD);
		if (ev_is_active(&m_data))
			ev_timer_stop(evLoop::at(m_tag), &m_data);
		m_data.data = NULL;
		m_verify = 0;
	}
	int evTimer::init(ev_tstamp itimer, ev_tstamp rtimer)
	{
		m_itimer = itimer;
		m_rtimer = rtimer;
		ev_timer_set(&m_data, m_itimer, m_rtimer);
		return 0;
	}
	int evTimer::init(ev_tstamp itimer)
	{
		return init(itimer, 0);
	}
	int evTimer::stop()
	{
		if (ev_is_active(&m_data))
			ev_timer_stop(evLoop::at(m_tag), &m_data);
		return 0;
	}
	int evTimer::again(ev_tstamp itimer, ev_tstamp rtimer)
	{
		m_itimer = itimer;
		m_rtimer = rtimer;
		if (itimer != 0 || rtimer != 0)
			ev_timer_set(&m_data, itimer, rtimer);
		ev_timer_again(evLoop::at(m_tag), &m_data);
		return 0;
	}
	int evTimer::again()
	{
		return again(0, 0);
	}
	int evTimer::active()
	{
		if (!ev_is_active(&m_data))
			ev_timer_start(evLoop::at(m_tag), &m_data);
		return 0;
	}
	bool evTimer::is_active()
	{
		return ev_is_active(&m_data);
	}
	ev_tstamp evTimer::remaining()
	{
		return ev_timer_remaining(evLoop::at(m_tag), &m_data);
	}
	int evTimer::start(ev_tstamp itimer, ev_tstamp rtimer)
	{
		init(itimer, rtimer);
		return active();
	}
	int evTimer::start(ev_tstamp itimer)
	{
		init(itimer);
		return active();
	}
}
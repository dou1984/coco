/*
 *	Copyright (c) 2019-2020 Zhao YunShan(93850592@qq.com)
 */
#include <signal.h>
#include <unistd.h>
#include "evLoop.h"
#include "evSignal.h"

namespace ashan
{
	void evSignal::_signal(struct ev_loop* loop, struct ev_signal* w, int revents)
	{
		evSignal* _this = (evSignal*)w->data;
		if (_this != NULL)
			_this->on_signal();
	}
	evSignal::evSignal(uint32_t id) : m_signal_id(id)
	{
		if (m_signal_id >= SIGNALMAX)
		{
			throw("evSignal signalid is error");
		}
		m_signal.data = this;
		ev_signal_init(&m_signal, evSignal::_signal, m_signal_id);
		ev_signal_start(evLoop::data(), &m_signal);
		m_tag = evLoop::tag();
	}
	evSignal::~evSignal()
	{
		ev_signal_stop(evLoop::at(m_tag), &m_signal);
	}
	int evSignal::signal()
	{
		static auto pid = getpid();
		kill(pid, m_signal_id);
		return 0;
	}
}
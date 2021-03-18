/*
 *	Copyright (c) 2019-2020 Zhao YunShan(93850592@qq.com)
 */
#ifndef __EVSIGNAL_H__
#define __EVSIGNAL_H__
#include <memory.h>
#include "Common.h"

#define SIGNALMIN (60)
#define SIGNALMAX (64)

namespace ashan
{
	class evSignal
	{
		uint32_t m_signal_id = 0;
		uint32_t m_tag = 0;
		struct ev_signal m_signal;
		static void _signal(struct ev_loop* loop, struct ev_signal* w, int revents);
	public:
		evSignal(uint32_t id);
		virtual ~evSignal();
		int signal();
		virtual int on_signal() { return 0; }
	public:
		int max() const { return SIGNALMAX; }
		int min() const { return SIGNALMIN; }
	};
}
#endif

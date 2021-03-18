/*
 *	Copyright (c) 2019-2020 Zhao YunShan(93850592@qq.com)
 */
#ifndef EVLOOP_H__
#define EVLOOP_H__
#include "Common.h"


#define max_ev_loop (0x1000)

namespace ashan
{
	class evLoop
	{
	public:
		static void start();
		static struct ev_loop* data();
		static struct ev_loop* at(uint32_t);
		static uint32_t tag();
	};
}
#endif


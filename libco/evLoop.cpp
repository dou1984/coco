/*
 *	Copyright (c) 2019-2020 Zhao YunShan(93850592@qq.com)
 */
#include <ThreadLocal.h>
#include <array>
#include "evLoop.h"
#include "coAsync.h"
#include "coAsyncMtx.h"

#define g_loop __this_ev_loop::ins()

namespace ashan
{
	static std::array<struct ev_loop*, max_ev_loop> all_loops = { nullptr };
	static std::atomic<uint32_t> g_index{ 0 };

	struct __ev_loop 
	{
		uint32_t m_tag = 0;
		struct ev_loop* m_loop = nullptr;		
		__ev_loop()
		{
			m_loop = ev_loop_new();
			m_tag = g_index++;
			all_loops[m_tag] = m_loop;			
		}
		~__ev_loop()
		{
			ev_loop_destroy(m_loop);
			all_loops[m_tag] = nullptr;
		}
		struct ev_loop* data()
		{
			return m_loop;
		}
		uint32_t tag()
		{
			return m_tag;
		}
	};

	struct 	__this_ev_loop : __ev_loop, coAsync, coAsyncMtx, ThreadLocal<__this_ev_loop>
	{
		__this_ev_loop() : __ev_loop(),
			coAsync(__ev_loop::m_loop, __ev_loop::m_tag),
			coAsyncMtx(__ev_loop::m_loop, __ev_loop::m_tag)
		{
		}
	};

	void evLoop::start()
	{
		ev_run(g_loop.data(), 0);
	}
	struct ev_loop* evLoop::data()
	{
		return g_loop.data();
	}
	struct ev_loop* evLoop::at(uint32_t _tag)
	{
		if (_tag < max_ev_loop)
			return all_loops[_tag];
		return nullptr;
	}
	uint32_t evLoop::tag()
	{
		return g_loop.tag();
	}
}
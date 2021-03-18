#include "coSleep.h"

namespace ashan
{
	coSleep::coSleep(ev_tstamp itime)
	{
		start(itime, 0);
	}
	int coSleep::on_timer()
	{
		assert(m_verify == MAGIC_WORD);
		//resume_ex();
		m_coro = nullptr;		
		std::coroutine_handle<> _awaiting = m_awaiting;
		if (_awaiting)
		{
			m_awaiting = nullptr;
			_awaiting.resume();
		}
		return 0;
	}
	coSleep:: ~coSleep()
	{
		m_coro = nullptr;
		m_awaiting = nullptr;
	}
}
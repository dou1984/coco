#include "coAsync.h"
#include "evLoop.h"

namespace ashan
{
	static coAsync* all_async[max_ev_loop] = { nullptr };

	coAsync::coAsync(struct ev_loop* _loop, uint32_t _tag) :evAsync(_loop, _tag)
	{
		all_async[_tag] = this;
	}
	int	coAsync::push_back(handle coro)
	{
		assert(!coro.done());
		m_list.push_back(coro);
		if (m_list.size() == 1)
		{
			resume();
		}		
		return 0;
	}
	int coAsync::trigger()
	{
		std::list<handle>::iterator it;
		while ((it = m_list.begin()) != m_list.end())
		{
			handle coro = *it;
			m_list.erase(it);
			coro.resume();
		}
		return 0;
	}
	int coAsync::erase(handle coro)
	{
		m_list.remove(coro);
		return 0;
	}
	coAsync* coAsync::at()
	{
		auto _tag = evLoop::tag();
		if (_tag < max_ev_loop)
		{
			return all_async[_tag];
		}
		return nullptr;
	}
}
#include "coAsyncMtx.h"
#include "evLoop.h"

namespace ashan
{
	static coAsyncMtx* all_async[max_ev_loop] = { nullptr };

	coAsyncMtx::coAsyncMtx(struct ev_loop* _loop, uint32_t _tag) :evAsync(_loop, _tag)
	{
		all_async[_tag] = this;
	}
	int	coAsyncMtx::push_back(std::coroutine_handle<> coro)
	{
		if (coro != NULL)
		{
			{
				std::lock_guard<std::mutex> _lock(m_mtx);
				m_list.push_back(coro);
			}
			resume();
		}
		return 0;
	}
	int coAsyncMtx::trigger()
	{
		decltype(m_list) _list;
		{
			std::lock_guard<std::mutex> _lock(m_mtx);
			_list.swap(m_list);
		}
		for (auto it = _list.begin(); it != _list.end();)
		{
			it->resume();
			it = _list.erase(it);
		}		
		return 0;
	}	
	coAsyncMtx* coAsyncMtx::_at(uint32_t _tag)
	{
		if (_tag < max_ev_loop)
		{
			return all_async[_tag];
		}
		return nullptr;
	}
}
#include "evAsync.h"
#include "evLoop.h"

namespace ashan
{	
	void evAsync::_Trigger(struct ev_loop* loop, ev_async* w, int revents)
	{
		if (revents & EV_ASYNC)
		{
			evAsync* _this = (evAsync*)w->data;
			if (_this != nullptr)
				_this->trigger();
		}
	}
	evAsync::evAsync()
	{
		m_Watcher.data = this;
		ev_async_init(&m_Watcher, _Trigger);
		ev_async_start(evLoop::data() , &m_Watcher);
		m_tag = evLoop::tag();
	}
	evAsync::evAsync(struct ev_loop* _loop, uint32_t _tag)
	{
		m_Watcher.data = this;
		ev_async_init(&m_Watcher, _Trigger);
		ev_async_start(_loop, &m_Watcher);
		m_tag = _tag;
	}
	evAsync::~evAsync()
	{		
		ev_async_stop(evLoop::at(m_tag), &m_Watcher);
	}
	int evAsync::resume()
	{
		ev_async_send(evLoop::at(m_tag), &m_Watcher);
		return 0;
	}
}
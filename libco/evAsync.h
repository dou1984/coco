#ifndef EVASYNC_H
#define EVASYNC_H
#include "Common.h"

namespace ashan
{
	class evAsync
	{
		ev_async m_Watcher;
		uint32_t m_tag = 0;		
		static void _Trigger(struct ev_loop* loop, ev_async* w, int revents);
	public:
		evAsync();
		evAsync(struct ev_loop* _loop, uint32_t _tag);
		virtual ~evAsync();
		int resume();
		virtual int trigger() { return 0; }
	};
}
#endif
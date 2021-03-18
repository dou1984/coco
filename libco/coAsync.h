#ifndef CO_ASYNC_H
#define CO_ASYNC_H
#include "evAsync.h"

namespace ashan
{
	class coAsync : public evAsync
	{
		using handle = std::coroutine_handle<>; 
		std::list<handle> m_list;		
	public:
		coAsync() = delete;
		coAsync(struct ev_loop* _loop, uint32_t _tag);
		int	push_back(handle coro);
		int erase(handle coro);
		int trigger();
		static coAsync* at();
	};
}

#endif
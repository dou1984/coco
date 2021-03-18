#ifndef CO_ASYNC_MTX_H
#define CO_ASYNC_MTX_H
#include "Common.h"
#include "evAsync.h"

namespace ashan
{
	class coAsyncMtx : public evAsync
	{
		std::list<std::coroutine_handle<>> m_list;
		std::mutex m_mtx;
		int trigger();
	public:
		coAsyncMtx() = delete;
		coAsyncMtx(struct ev_loop* _loop, uint32_t _tag);
		int	push_back(std::coroutine_handle<> coro);
		static coAsyncMtx* _at(uint32_t _tag);
	};
}
#endif
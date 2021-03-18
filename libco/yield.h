#ifndef CO_YIELD_H
#define CO_YIELD_H
#include <functional>

namespace ashan
{
	template <typename _Ret>
	struct yield : awaiter_disabled
	{
		struct promise_type;
		using handle_type = std::coroutine_handle<promise_type>;
		handle_type coro{ nullptr };		
		yield(handle_type h) : coro(h)
		{
		}
		~yield()
		{
		}	
		yield(yield&& o)
		{
			std::swap(coro, o.coro);
		}
		const yield& operator=(yield&& o)
		{
			std::swap(coro, o.coro);
			return *this;
		}		
		_Ret get()
		{
			return coro.promise().value;
		}
		void resume()
		{
			coro.resume();
		}
		_Ret await_resume()
		{
			return coro.promise().value;
		}		
		template<class... _Args>
		static yield go( _Args&&... _args)
		{
			_Ret r;
			while ((r = std::invoke(std::forward<_Args>(_args)...)))
				co_yield r;
			co_return r;
		}
		struct promise_type : promise_never_never, promise
		{
			_Ret value;
			yield<_Ret> get_return_object()
			{
				return { handle_type::from_promise(*this) };
			}
			std::suspend_never return_value(_Ret v)
			{
				value = v;
				return {};
			}
			std::suspend_always yield_value(_Ret v)
			{
				value = v;
				return {};
			}
		};			
	};
}
#endif
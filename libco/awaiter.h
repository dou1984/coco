#ifndef CO_AWAITER_H
#define CO_AWAITER_H
#include <coroutine>
#include "coList.h"
#include "promise.h"

namespace ashan
{	
	template<class _Ret = bool, bool _Ready = true, class _Promise = promise_never_never>
	struct awaiter
	{
		struct promise_type : promise, _Promise
		{
			_Ret value;			
			awaiter get_return_object()
			{				
				return { std::coroutine_handle<promise_type>::from_promise(*this) };
			}						
			std::suspend_always yield_value(_Ret&& v)
			{
				value = std::forward<_Ret>(v);
				return {};
			}
			std::suspend_never return_value(_Ret&& v)
			{
				value = std::forward<_Ret>(v);						
				return {};
			}			
			void return_void()
			{			
			}
		};		
		awaiter() = default;
		awaiter(std::coroutine_handle<promise_type> h) : m_coro(h)
		{						
		}		
		awaiter(awaiter&& o)
		{
			std::swap(m_coro, o.m_coro);		
			std::swap(m_awaiting, o.m_awaiting);
		}
		awaiter(const awaiter&) = delete;
		const awaiter& operator = (awaiter&&) = delete;		
		virtual ~awaiter() = default;
		void resume()
		{
			if (m_coro && !m_coro.done())
				m_coro.resume();
		}		
		void resume_ex()
		{
			if (m_awaiting && !m_awaiting.done())
				m_awaiting.resume();
		}
		bool done()
		{
			return m_coro ? m_coro.done() : true;
		}
		bool await_ready()
		{
			return _Ready; 
		}
		void await_suspend(std::coroutine_handle<> awaiting)
		{
			m_awaiting = awaiting;			
		}
		auto await_resume() 
		{			
			return m_coro ? m_coro.promise().value : _Ret{};
		}		
		std::coroutine_handle<promise_type> m_coro = nullptr;		
		std::coroutine_handle<> m_awaiting = nullptr;
	};

	struct awaiter_disabled
	{
		bool await_ready() = delete;
		void await_suspend(std::coroutine_handle<> awaiting) = delete;
		void await_resume() = delete;
	};		
}
#endif
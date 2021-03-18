#ifndef CO_PROMISE_H
#define CO_PROMISE_H
#include <coroutine>
#include <iostream>
#include <string.h>

namespace ashan
{
	struct promise 
	{
		promise()
		{			
		}
		~promise()
		{
		}
		void unhandled_exception()
		{			
			throw std::current_exception();
		}		
	};
	struct promise_never_never
	{
		std::suspend_never initial_suspend()
		{		
			return {};
		}
		std::suspend_never final_suspend()
		{			
			return {};
		}
	};
	struct promise_always_never
	{
		std::suspend_always initial_suspend()
		{
			return {};
		}
		std::suspend_never final_suspend()
		{
			return {};
		}
	};
	struct promise_never_always
	{
		std::suspend_never initial_suspend()
		{
			return {};
		}
		std::suspend_always final_suspend()
		{
			return {};
		}
	};
	struct promise_always_always
	{
		std::suspend_always initial_suspend()
		{
			return {};
		}
		std::suspend_always final_suspend()
		{
			return {};
		}
	};		
	using pnn = promise_never_never;
	using pan = promise_always_never;
	using pna = promise_never_always;
	using paa = promise_always_always;
}
#endif

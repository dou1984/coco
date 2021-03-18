#ifndef CO_TASK_H
#define CO_TASK_H
#include <assert.h>
#include <list>
#include "coList.h"
#include "promise.h"
#include "awaiter.h"

namespace ashan
{	
	class coTask final 
	{
		struct When;
	public:
		struct promise_type : promise, promise_never_never
		{
			coTask* m_task = nullptr;
			When* m_when = nullptr;
			size_t m_verify = MAGIC_WORD;
			coTask get_return_object();
			void return_void();
			void notify();
			~promise_type();
			
		};
		using handle = std::coroutine_handle<promise_type>;
	public:
		coTask(handle h);
		~coTask();
		coTask(coTask&& o) = delete;		
		coTask(const coTask&) = delete;		
		void resume();
		template<class... _Event>
		static auto wait_for_all(_Event&&... e)
		{
			When w;			
			w.m_flag.set(When::e_when_all);
			(w.co_insert(&e), ...);			
			return w;
		}
		template<class... _Event>
		static auto wait_for(_Event&&... e)
		{
			When w;			
			(w.co_insert(&e), ...);			
			return w;
		}		
	private:		
		handle m_coro = nullptr;
		size_t m_verify = MAGIC_WORD;
	private:
		void notify();
		void destroy();
		bool done();
		void check();
		struct When : awaiter<size_t, false>
		{
			enum e_when
			{
				e_when_terminated,
				e_when_all,
				e_when_end,
			};
			using base = awaiter<size_t, false>;
			std::list<handle> m_list;
			std::bitset<e_when_end> m_flag;
			uint32_t m_tag = 0;
			size_t m_verify = MAGIC_WORD;
			When();
			When(When&& o);
			~When();
			void co_insert(coTask* o);
			void co_erase(handle l);			
			void erase(handle l);
			bool await_ready();			
		};
	};
}
#endif

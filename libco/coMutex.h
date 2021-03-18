#ifndef CO_MUTEX_H
#define CO_MUTEX_H
#include <coroutine>
#include <atomic>
#include <cstdint>
#include <mutex> 
#include <cassert>
#include "Common.h"
#include "coList.h"
#include "awaiter.h"
#include "coSpinlock.h"

namespace ashan
{	
	class coMutex
	{	
		struct awtMutex;		
		struct awtAutoLock;
	public:
		coMutex() = default;				
		bool try_lock();		
		awtAutoLock auto_lock();		
		awtMutex lock();
		void unlock();			
	private:		
		void co_insert(coList* l);			
		coList m_list;	
		coSpinlock m_state;
		coSpinlock m_mtx;
	private:
		struct relMutex
		{
			coMutex* m_mutex = nullptr;
			relMutex(coMutex*);
			relMutex(const coMutex&) = delete;
			relMutex(relMutex&&);
			~relMutex();
		};
		struct awtMutex : coList
		{
			coMutex* m_mutex = nullptr;
			std::coroutine_handle<> m_awaiting = nullptr;
			size_t m_verify = MAGIC_WORD;
			uint32_t m_tag;			
			awtMutex(coMutex* mutex);
			awtMutex(const awtMutex&) = delete;
			awtMutex(awtMutex&& o) = delete;
			~awtMutex();
			bool await_ready();
			void await_suspend(std::coroutine_handle<> awaiting);
			void await_resume();
			void resume_ex();
		};
		struct awtAutoLock : awtMutex
		{
			using awtMutex::awtMutex;
			relMutex await_resume();
		};
	};	
}
#endif

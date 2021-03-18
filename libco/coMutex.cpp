#include "coMutex.h"
#include "evLoop.h"
#include "coAsync.h"
#include "coAsyncMtx.h"

namespace ashan
{
	bool coMutex::try_lock()
	{
		return m_state.try_lock();
	}
	coMutex::awtAutoLock coMutex::auto_lock()
	{
		return { this };
	}
	coMutex::awtMutex coMutex::lock()
	{
		return { this };
	}
	void coMutex::unlock()
	{
		m_mtx.lock();
		auto _awt = (awtMutex*)m_list.begin();
		if (_awt == m_list.end())
		{
			m_mtx.unlock();
			m_state.unlock();
		}
		else
		{
			m_list.erase(_awt);			
			m_mtx.unlock();
			_awt->resume_ex();
		}
	}
	void coMutex::co_insert(coList* l)
	{
		m_mtx.lock();
		m_list.push_back(l);		
		m_mtx.unlock();
	}
	coMutex::relMutex::relMutex(coMutex* _mtx)
	{
		m_mutex = _mtx;		
	}
	coMutex::relMutex::relMutex(relMutex&& o)
	{
		std::swap(m_mutex, o.m_mutex);
	}
	coMutex::relMutex::~relMutex()	
	{
		if (m_mutex)
			m_mutex->unlock();		
	}
	coMutex::awtMutex::awtMutex(coMutex* mutex) : m_mutex(mutex)
	{
		m_tag = evLoop::tag();
	}
	coMutex::awtMutex::~awtMutex()
	{
		assert(m_verify == MAGIC_WORD);
#ifdef DELAY_RESUME		
		if (m_awaiting)		
			coAsync::at()->erase(m_awaiting);		
#endif
		m_awaiting = nullptr;
		if (!empty())
			m_mutex->m_list.erase(this);
		m_verify = 0;
	}
	bool coMutex::awtMutex::await_ready()
	{
		assert(m_verify == MAGIC_WORD);
		if (m_mutex->try_lock())
			return true;
		m_mutex->co_insert(this);		
		return false;
	}
	void coMutex::awtMutex::await_suspend(std::coroutine_handle<> awaiting)
	{
		m_awaiting = awaiting;
	}
	void coMutex::awtMutex::await_resume()
	{		
	}
	coMutex::relMutex coMutex::awtAutoLock::await_resume()
	{
		return { m_mutex };
	}
	void coMutex::awtMutex::resume_ex()
	{
		if (m_awaiting)
		{
#ifdef DELAY_RESUME
			coAsync::at()->push_back(m_awaiting);//如果m_awaiting在切换线程中释放，可能会崩溃
#else
			m_awaiting.resume();
#endif
		}
	}
	
}
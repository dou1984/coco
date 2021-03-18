#include "Common.h"
#include "coTask.h"
#include "coAsync.h"
#include "coAsyncMtx.h"
#include "evcpp.h"

namespace ashan
{
	coTask::When::When()
	{
		m_tag = evLoop::tag();
	}
	coTask::When::When(When&& o) : base(std::move(o))
	{	
		m_list.splice(m_list.end(), o.m_list);
		std::swap(m_flag, o.m_flag);
	}
	coTask::When::~When()
	{
		assert(m_verify == MAGIC_WORD);
		m_flag.set(e_when_terminated);
		for (auto coro : m_list)
		{
			assert(coro.promise().m_verify == MAGIC_WORD);
			coro.promise().m_when = nullptr;
			coro.destroy();
		}
		m_verify = 0;
	}
	void coTask::When::co_erase(handle coro)
	{	
		assert(coro.promise().m_verify == MAGIC_WORD);
		assert(m_verify == MAGIC_WORD);
		erase(coro);
		if (m_flag[e_when_terminated])[[unlikely]]
		{
			return;
		}
		if (m_list.empty())
		{
			resume_ex();
		}		
		else if (!m_flag[e_when_all])
		{
#ifdef DELAY_RESUME
			coAsync::at()->push_back(m_awaiting);
#else
			m_awaiting.resume();
#endif
			m_flag.set(e_when_terminated);
		}
	}
	void coTask::When::erase(coTask::handle coro)
	{
		assert(coro.promise().m_verify == MAGIC_WORD);
		m_list.remove(coro);
	}
	void coTask::When::co_insert(coTask* o)
	{
		assert(o->m_verify == MAGIC_WORD);
		if (!o->done())
		{
			o->m_coro.promise().m_when = this;		
			m_list.push_back(o->m_coro);
		}
	}
	bool coTask::When::await_ready()
	{
		return m_list.empty();
	}
	coTask coTask::promise_type::get_return_object()
	{
		return { std::coroutine_handle<promise_type>::from_promise(*this) };
	}
	void coTask::promise_type::return_void()
	{		
		if (m_task && m_task->m_coro)
		{
			m_task->m_coro = nullptr;
			m_task = nullptr;
		}
		notify();
	}
	void coTask::promise_type::notify()
	{
		if (m_when)
		{
			auto coro = handle::from_promise(*this);
			m_when->co_erase(coro);
		}
	}		
	coTask::promise_type::~promise_type()
	{
		assert(m_verify == MAGIC_WORD);		
		if (m_task && m_task->m_coro)
		{
			m_task->check();
			m_task->m_coro = nullptr;
		}
		m_when = nullptr;
		m_verify = 0;
	}
	coTask::coTask(handle h) :m_coro(h)
	{
		m_coro.promise().m_task = this;
	}
	coTask::~coTask()
	{
		assert(m_verify == MAGIC_WORD);
		if (m_coro)
		{
			check();
			m_coro.promise().m_task = nullptr;
			m_coro = nullptr;
		}
		m_verify = 0;
	}
	void coTask::resume()
	{
		assert(m_verify == MAGIC_WORD);
		if (m_coro)
			m_coro.resume();
	}
	void coTask::destroy()
	{
		assert(m_verify == MAGIC_WORD);
		if (m_coro)
		{
			assert(m_coro.promise().m_verify == MAGIC_WORD);
			m_coro.destroy();	
			check();
			m_coro = nullptr;
		}
	}
	bool coTask::done()
	{
		return m_coro ? m_coro.done() : true;
	}
	void coTask::check()
	{
#ifdef DELAY_RESUME
		coAsync::at()->erase(m_coro);
#endif
	}
}
#ifndef CO_TRIGGER_H
#define CO_TRIGGER_H
#include <unordered_map>
#include <string>
#include "Common.h"
#include "coList.h"
#include "awaiter.h"

namespace ashan
{	
	template<class _Key>
	class coTrigger
	{
	public:
		struct awaiter
		{
			_Key m_id;
			coTrigger* m_trigger = nullptr;
			std::coroutine_handle<> m_awaiting = nullptr;
			size_t m_verify = MAGIC_WORD;
			awaiter(coTrigger* _trg, const _Key& _id) : m_id(_id), m_trigger(_trg)
			{
				m_trigger->m_triggers.emplace(_id, this);
			}
			awaiter(const awaiter&) = delete;
			awaiter(awaiter&&) = delete;
			~awaiter()
			{
				assert(m_verify == MAGIC_WORD);
				m_awaiting = nullptr;
				if (m_trigger)
					m_trigger->m_triggers.erase(m_id);
				m_trigger = nullptr;
				m_verify = 0;
			}
			const _Key& await_resume()
			{
				return m_id; 
			}
			bool await_ready() 
			{
				return false;
			}
			void await_suspend(std::coroutine_handle<> awaiting) 
			{
				m_awaiting = awaiting; 
			}
			void resume_ex()
			{
				if (m_awaiting)
					m_awaiting.resume();
			}
			void destroy()
			{
				if (m_awaiting)
				{
					auto _awaiting = m_awaiting;
					m_awaiting = nullptr;
					_awaiting.destroy();
				}
			}
		};
	public:		
		bool exist(const _Key& _id)
		{
			return m_triggers.find(_id) != m_triggers.end(); 
		}
		void destroy(const _Key& _id)
		{
			if (auto it = m_triggers.find(_id); it != m_triggers.end())
			{
				auto _awt = it->second;
				if (_awt)
					_awt->destroy();
				m_triggers.erase(it);
			}
		}
		void resume(const _Key& _id)
		{
			if (auto it = m_triggers.find(_id); it != m_triggers.end())
			{
				auto _awt = it->second;
				_awt->resume_ex();
			}
		}
		awaiter suspend(const _Key& _id)
		{
			return { this, _id };
		}
	private:		
		std::unordered_map<_Key, awaiter*> m_triggers;		
	};
}

#endif
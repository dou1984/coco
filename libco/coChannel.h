#ifndef CO_CHANNEL_H
#define CO_CHANNEL_H
#include <list>
#include "Common.h"
#include "coList.h"
#include "awaiter.h"
#include "coSpinlock.h"
#include "coAsync.h"

namespace ashan
{
	template<class _T>
	class coChannel final
	{	
		struct _Write;
		struct _Read;				
	public:
		_Read read(_T& _data)
		{			
			assert(m_verify == MAGIC_WORD);
			return { this, _data };
		}
		_Write write(_T&& _data)
		{
			assert(m_verify == MAGIC_WORD);
			return { this, std::move(_data), false };
		}
		_Write write(_T& _data)
		{
			assert(m_verify == MAGIC_WORD);
			return { this, _data, false };
		}
		_Write async_write(_T&& _data)
		{
			assert(m_verify == MAGIC_WORD);
			return { this, std::move(_data), true };
		}	
		_Write async_write(_T& _data)
		{
			assert(m_verify == MAGIC_WORD);
			return { this, _data, true };
		}
		~coChannel()
		{
			assert(m_verify == MAGIC_WORD);
			coList* it;
			std::lock_guard<std::mutex> _lock(m_mtx);			
			while ((it = m_read.begin()) != m_read.end())
			{
				m_read.erase(it);
				auto _awt = (_Read*)(it);
				if (_awt->m_awaiting)
				{
					_awt->m_awaiting.destroy();
					_awt->m_awaiting = nullptr;
					_awt->m_channel = nullptr;
				}
			}
			while ((it = m_write.begin()) != m_write.end())
			{
				m_write.erase(it);
				auto _awt = (_Write*)(it);
				if (_awt->m_awaiting)
				{
					_awt->m_awaiting.destroy();
					_awt->m_awaiting = nullptr;
					_awt->m_channel = nullptr;
				}
				if (_awt->m_async)
				{
					delete _awt;
				}
			}
			m_verify = 0;
		}
	private:
		coList m_read;
		coList m_write;		
		std::mutex m_mtx;
		size_t m_verify = MAGIC_WORD;
	private:		
		struct _Read : coList
		{
			coChannel* m_channel;
			_T& m_data;
			size_t m_verify = MAGIC_WORD;
			std::coroutine_handle<> m_awaiting = nullptr;
			_Read(coChannel* c, _T& d) : m_channel(c), m_data(d)
			{
			}
			~_Read()
			{
				assert(m_verify == MAGIC_WORD);
#ifdef DELAY_RESUME
				if (m_awaiting)				
					coAsync::at()->erase(m_awaiting);
#endif
				m_awaiting = nullptr;			
				if (!empty())
				{
					m_channel->m_mtx.lock();
					m_channel->m_read.erase(this);
					m_channel->m_mtx.unlock();					
				}	
				m_verify = 0;
			}
			bool await_ready()
			{
				m_channel->m_mtx.lock();
				if (m_channel->m_write.empty())
				{
					m_channel->m_read.push_back(this);
					m_channel->m_mtx.unlock();
					return false;
				}
				auto _awt = (_Write*)m_channel->m_write.front();
				m_channel->m_write.erase(_awt);
				m_channel->m_mtx.unlock();
				m_data = std::move(_awt->m_data);
				if (_awt->m_async)
					delete _awt;
				else
					_awt->resume();
				return true;
			}
			void await_suspend(std::coroutine_handle<> awaiting)
			{
				m_awaiting = awaiting;
			}
			void await_resume()
			{
			}
			void resume()
			{
				if (m_awaiting)
				{
#ifdef DELAY_RESUME
					coAsync::at()->push_back(m_awaiting);
#else
					m_awaiting.resume();
#endif
				}
			}
		};
		struct _Write : coList
		{
			bool m_async = false;
			coChannel* m_channel = nullptr;
			_T m_data;
			std::coroutine_handle<> m_awaiting = nullptr;
			size_t m_verify = MAGIC_WORD;
			_Write(coChannel* c, _T&& d, bool _async) : m_async(_async), m_channel(c), m_data(std::move(d))
			{
			}
			_Write(coChannel* c, _T& d, bool _async) : m_async(_async), m_channel(c), m_data(d)
			{
			}
			_Write(_Write&& o)
			{				
				m_async = o.m_async;
				m_channel = o.m_channel;//此处需要copy，不能swap
				m_data = std::forward<_T>(o.m_data);
			}
			~_Write()
			{
				assert(m_verify == MAGIC_WORD);
#ifdef DELAY_RESUME
				if (m_awaiting)				
					coAsync::at()->erase(m_awaiting);				
#endif
				m_awaiting = nullptr;
				if (!empty())
				{
					m_channel->m_mtx.lock();
					m_channel->m_write.erase(this);
					m_channel->m_mtx.unlock();
				}		
				m_verify = 0;
			}			
			bool await_ready()
			{
				m_channel->m_mtx.lock();
				if (m_channel->m_read.empty())
				{
					if (m_async)
						m_channel->m_write.push_back(new _Write(std::move(*this)));
					else
						m_channel->m_write.push_back(this);
					m_channel->m_mtx.unlock();
					return m_async;
				}
				auto t = (_Read*)(m_channel->m_read.front());
				m_channel->m_read.erase(t);
				m_channel->m_mtx.unlock();
				t->m_data = std::move(m_data);				
				t->resume();
				return true;
			}
			void await_suspend(std::coroutine_handle<> awaiting)
			{
				m_awaiting = awaiting;
			}
			void await_resume()
			{
			}
			void resume()
			{
				if (m_awaiting)
				{
#ifdef DELAY_RESUME
					coAsync::at()->push_back(m_awaiting);
#else
					m_awaiting.resume();
#endif
				}
			}
		};
	};
}

#endif
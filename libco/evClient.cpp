/*
 *	Copyright (c) 2019-2020 Zhao YunShan(93850592@qq.com)
 */
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "evLoop.h"
#include "evClient.h"
#include "coMsg.h"

#define DEFBUFLEN (0x10000)
namespace ashan
{
	void evClient::_cb_read(struct ev_loop* loop, struct ev_io* w, int revents)
	{
		evClient* _this = (evClient*)w->data;
		assert(_this != NULL);		
		assert(_this->m_fd != INVALID);
		assert(_this->m_verify == MAGIC_WORD);
		auto& _buf = _this->m_ReadBuffer;
		int iCount = _buf.append([=](auto _buffer, auto _size) {
			return ::recv(_this->m_fd, _buffer, _size, 0);
			});		
		if (iCount > 0)
		{
			_this->on_recv();
		}
		else if (iCount == 0)
		{
			_this->_close();
		}
		else if (errno == EAGAIN || errno == EINPROGRESS || errno == EINTR)
		{
		}
		else
		{
			_this->_close();
		}
	}
	void evClient::_cb_write(struct ev_loop* loop, struct ev_io* w, int revents)
	{
		evClient* _this = (evClient*)w->data;
		assert(_this != NULL);
		if (_this->m_fd != INVALID)
		{
			auto& _list = _this->m_WriteBuffer;
			while (!_list.empty())
			{
				auto& _buf = _list.front();
				auto r = ::send(_this->m_fd, _buf.data(), _buf.size(), 0);
				if (r >= 0)
				{
					_list.pop_front();					
				}
				else if (!(errno == EAGAIN || errno == EINPROGRESS || errno == EINTR))
				{
					_this->_close();
				}
				else
				{
					ev_io_start(EV_A_ w);
					return;
				}
				assert(r != 0);
			}
		}
		ev_io_stop(EV_A_ w);
	}
	int evClient::_send()
	{
		assert(m_verify == MAGIC_WORD);
		if (m_WriteBuffer.size() > 0)
			ev_io_start(evLoop::at(m_tag), &m_Write);
		return 0;
	}
	int evClient::_clear()
	{
		m_Close = nullptr;
		return 0;
	}
	int evClient::_close()
	{		
		if (m_fd == INVALID)
		{
			return  0;
		}
		int fd = m_fd;
		::close(m_fd);
		m_fd = INVALID;
		ev_io_stop(evLoop::at(m_tag), &m_Read);
		ev_io_stop(evLoop::at(m_tag), &m_Write);
		return on_close(fd);
	}
	bool evClient::valid()
	{
		return m_fd != INVALID;
	}
	evClient::evClient(int fd, VClose* _Close) : m_fd(fd), m_Close(_Close)
	{
		m_Read.data = this;
		ev_io_init(&m_Read, evClient::_cb_read, m_fd, EV_READ);
		ev_io_start(evLoop::data(), &m_Read);

		m_Write.data = this;
		ev_io_init(&m_Write, evClient::_cb_write, m_fd, EV_WRITE);

		m_tag = evLoop::tag();
	}
	evClient::~evClient()
	{
		assert(m_verify == MAGIC_WORD);
		if (m_fd != INVALID)
		{
			::close(m_fd);
			m_fd = INVALID;
		}
		m_Read.data = NULL;
		ev_io_stop(evLoop::at(m_tag), &m_Read);

		m_Write.data = NULL;
		ev_io_stop(evLoop::at(m_tag), &m_Write);
		m_verify = 0;
	}
	int evClient::_send(ioBuffer<char>& _buf)
	{
		assert(m_verify == MAGIC_WORD);
		/*
		m_WriteBuffer.emplace_back(std::move(_buf));
		if (m_fd >= 0)
		{
			ev_io_start(evLoop::at(m_tag), &m_Write);
			return 0;
		}
		return INVALID;
		*/
		if (m_fd < 0) [[unlikely]]
		{
			m_WriteBuffer.emplace_back(std::move(_buf));
			return 0;
		}
			if (m_WriteBuffer.size() > 0) [[unlikely]]
			{
				m_WriteBuffer.emplace_back(std::move(_buf));
				ev_io_start(evLoop::at(m_tag), &m_Write);
				return 0;
			}
		auto r = ::send(m_fd, _buf.data(), _buf.size(), 0);
		if (r < 0) [[unlikely]]
		{
			if (errno == EAGAIN || errno == EINPROGRESS || errno == EINTR)
			{
				m_WriteBuffer.emplace_back(std::move(_buf));
				ev_io_start(evLoop::at(m_tag), &m_Write);
				return 0;
			}
			else
			{
				_close();
			}
		}
		return r;
	}
	int evClient::_send(const char* buffer, int _size)
	{
		ioBuffer<char> b(_size);
		b.push_back(buffer, _size);
		return _send(b);
	}
	int evClient::on_recv()
	{
		return 0;
	}
	int evClient::on_close(int fd)
	{
		assert(fd != INVALID);
		assert(m_verify == MAGIC_WORD);
		if (m_Close)
			m_Close->on_close(fd);		
		return 0;
	}
}
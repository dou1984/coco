/*
 *	Copyright (c) 2019-2020 Zhao YunShan(93850592@qq.com)
 */
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "evConnect.h"
#include "evLoop.h"
#include "evAddr.h"
#include "CLog.h"
#include "basesocket.h"

namespace ashan
{
	evConnect::evConnect()
	{
		m_tag = evLoop::tag();
	}
	evConnect::~evConnect()
	{
		assert(m_verify == MAGIC_WORD);
		m_verify = 0;
	}
	void evConnect::_cb_connect(struct ev_loop* loop, struct ev_io* w, int revents)
	{
		if (EV_ERROR & revents)
		{			
			return;
		}
		evConnect* _this = (evConnect*)(w->data);
		if (_this != NULL)
			_this->_connect();
	}
	void evConnect::_connect()
	{
		std::string m_ip;
		int m_port;
		EVADDR.search(m_Addr, m_ip, m_port);
		auto err = GetSocketError(m_fd);
		if (err == 0)
		{
			on_connect(m_fd, m_ip.c_str(), m_port);
		}
		else
		{
			int fd = m_fd;
			close(m_fd);
			m_fd = INVALID;
			err_connect(fd, m_ip.c_str(), m_port);
		}
		ev_io_stop(evLoop::at(m_tag), &m_Read);
		ev_io_stop(evLoop::at(m_tag), &m_Write);
	}
	int evConnect::connect(const char* _ip, int _port)
	{
		if (m_fd >= 0)
		{			
			return m_fd;
		}
		m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
		if (m_fd < 0) [[unlikely]]
		{
			return m_fd;
		}
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(_port);
		addr.sin_addr.s_addr = inet_addr(_ip);
		if (addr.sin_addr.s_addr == INADDR_NONE)
		{			
		error_return:
			close(m_fd);
			m_fd = INVALID;
			return m_fd;
		}

		m_Addr = EVADDR.combine(_ip, _port);
		fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL, 0) | O_NONBLOCK);
		if (::connect(m_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		{
			if (errno == EINPROGRESS)
			{
				m_Read.data = this;
				ev_io_init(&m_Read, &evConnect::_cb_connect, m_fd, EV_READ);
				ev_io_start(evLoop::at(m_tag), &m_Read);

				m_Write.data = this;
				ev_io_init(&m_Write, &evConnect::_cb_connect, m_fd, EV_WRITE);
				ev_io_start(evLoop::at(m_tag), &m_Write);
				return m_fd;
			}			
			goto error_return;
		}
		return m_fd;
	}
}
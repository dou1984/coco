/*
 *	Copyright (c) 2019-2020 Zhao YunShan(93850592@qq.com)
 */
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <map>
#include "evServer.h"
#include "evLoop.h"
#include "ioChannel.h"

static std::map<std::string, std::pair<int, int>> g_addr_fd;
static std::mutex g_mutex;

namespace ashan
{
	void evServer::_accept(struct ev_loop* loop, struct ev_io* w, int revents)
	{
		if (EV_ERROR & revents)
		{			
			return;
		}

		struct sockaddr_in c;
		socklen_t len = sizeof(c);
		int fd = ::accept(w->fd, (struct sockaddr*)&c, &len);
		if (fd < 0)
		{			
			return;
		}
		fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
		evServer* _this = (evServer*)(w->data);
		if (_this != NULL)
			_this->on_accept(fd);
	}

	int evServer::_start(const char* ip, int port)
	{
		assert(m_Addr == "");
		m_Addr = EVADDR.combine(ip, port);
		std::lock_guard<std::mutex> _lock(g_mutex);
		auto it = g_addr_fd.find(m_Addr);
		if (it != g_addr_fd.end())
		{
			m_fd = it->second.first;
			it->second.second++;
			return m_fd;
		}

		m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
		if (m_fd < 0) [[unlikely]]
		{			
		__error_return__:
			close(m_fd);
			m_fd = INVALID;
			return m_fd;
		}

		int on = 1;
		if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) [[unlikely]]
		{			
			goto __error_return__;
		}

		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ip);
		if (addr.sin_addr.s_addr == INADDR_NONE)
		{			
			goto __error_return__;
		}

		if (bind(m_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		{			
			goto __error_return__;
		}

		fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL, 0) | O_NONBLOCK);
		if (listen(m_fd, 1000) < 0)
		{			
			goto __error_return__;
		}
		g_addr_fd[m_Addr] = std::make_pair(m_fd, 1);		
		return m_fd;
	}
	evServer::~evServer()
	{
		stop();
	}
	int evServer::start(const char* ip, int port)
	{
		if (m_fd >= 0)
			return m_fd;
		_start(ip, port);
		if (m_fd == INVALID)
			return m_fd;
		m_Accept.data = this;
		ev_io_init(&m_Accept, evServer::_accept, m_fd, EV_READ);
		ev_io_start(evLoop::data(), &m_Accept);
		
		IOCH.m_addr = m_Addr;
		return m_fd;
	}
	int evServer::stop()
	{
		if (m_fd != INVALID)
		{
			ev_io_stop(evLoop::data(), &m_Accept);
			m_Accept.data = NULL;
			m_fd = INVALID;

			std::lock_guard<std::mutex> _lock(g_mutex);
			auto it = g_addr_fd.find(m_Addr);
			if ((--it->second.second) == 0)
			{
				close(it->second.first);
				g_addr_fd.erase(it);
			}
			m_Addr.clear();
		}
		return 0;
	}
	int evServer::on_accept(int fd)
	{
		return 0;
	}
}
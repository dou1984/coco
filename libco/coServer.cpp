#include "coServer.h"
#include "coMsg.h"
#include "ioChannel.h"
#include "coTcp.h"

namespace ashan
{
	coServer::awtAccept coServer::co_accept()
	{
		return { this };
	}
	int coServer::on_accept(int fd)
	{
		auto c = std::make_shared<coClient>(fd, this);
		m_clients.emplace(fd, c);

		if (auto it = m_list.begin(); it != m_list.end())
		{
			m_list.erase(it);
			awtAccept* io = (awtAccept*)(it);
			io->_client = c;
			io->resume_ex();
		}		
		return 0;
	}
	int coServer::on_close(int fd)
	{
		m_clients.erase(fd);
		return 0;
	}
	coServer::~coServer()
	{
		for (auto& it : m_clients)
		{
			it.second->_clear();
		}
	}
	coServer::awtAccept::awtAccept(coServer* o) : _this(o)
	{
		_this->m_list.push_back(this);
	}
	coServer::awtAccept::~awtAccept()
	{
		_this->m_list.erase(this);
	}
	bool coServer::awtAccept::await_ready()
	{
		return false;
	}
	void coServer::awtAccept::await_suspend(std::coroutine_handle<> awaiting)
	{
		m_awaiting = awaiting;
	}
	std::shared_ptr<coClient> coServer::awtAccept::await_resume()
	{
		return _client;		
	}
	void coServer::awtAccept::resume_ex()
	{
		if (m_awaiting)
			m_awaiting.resume();
	}
	/*
	coTask coServer::co_echo(const char* ip, int port)
	{		
		start(ip, port);
		while (true)
		{
			auto c = co_await co_accept();

			auto r = co_await c->co_read(RPC_INVALID);
			if (r < 0)
				continue;

			auto msg = c->get_data();

			auto b = sayhi();

			co_await c->co_write(b);

			IOCH.on_init(msg->data, c);

			co_heartbeat(c, 6.0);	
			 			
			//try_process(c);

		}
	}	
	*/

}
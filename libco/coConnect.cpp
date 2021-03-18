#include "coConnect.h"
#include "ioChannel.h"
#include "coTcp.h"

namespace ashan
{	
	coConnect::awtConnect::awtConnect(coConnect* o, int fd) :_this(o), _fd(fd)
	{
		_this->m_list.push_back(this);
	}
	coConnect::awtConnect::~awtConnect()
	{
		_this->m_list.erase(this);
	}
	bool coConnect::awtConnect::await_ready()
	{
		return _fd == INVALID;
	}
	void coConnect::awtConnect::await_suspend(std::coroutine_handle<> awaiting)
  	{		
		m_awaiting = awaiting;
	}
	std::shared_ptr<coClient> coConnect::awtConnect::await_resume()
	{		
		return _this->m_client;
	}
	coConnect::~coConnect()
	{
		if (m_client)
			m_client->_clear();
	}
	coConnect::awtConnect coConnect::co_connect(const char* ip, int port)
	{
		int fd = evConnect::connect(ip, port);
		return { this , fd };
	}
	void coConnect::awtConnect::resume_ex()
	{
		if (m_awaiting)
			m_awaiting.resume();
	}	
	int coConnect::on_connect(int fd, const char* ip, int port)
	{
		m_client = std::make_shared<coClient>(fd, this);
		
		auto _addr =  EVADDR.combine(ip, port);		

		IOCH.on_init(_addr, m_client);

		resume(fd);		
		return 0;
	}
	int coConnect::err_connect(int fd, const char* _ip, int _port)
	{		
		resume(fd);
		return 0;
	}
	int coConnect::on_close(int fd)
	{
		m_client.reset();
		m_fd = INVALID;
		return 0;
	}
	std::shared_ptr<coClient>& coConnect::get()
	{		
		return m_client;
	}
	void coConnect::resume(int fd)
	{
		coList* it;		
		while((it = m_list.begin()) != m_list.end())
		{			
			if (awtConnect* c = (awtConnect*)(it); c->_fd == fd)
			{
				m_list.erase(c);
				c->resume_ex();
				break;
			}
		}		
	}
// 	int coConnect::set(const coFunc& _process)
// 	{
// 		co_process = _process;
// 		return 0;
// 	}
}
#include "ioChannel.h"
#include "coSleep.h"
#include "coTcp.h"
#include "coConnect.h"

namespace ashan
{	
	ioControl ioChannel::get(const std::string& addr)
	{
		ioControl ctl;		
		if (auto it = m_clients.find(addr); it != m_clients.end())
		{			
			ctl.m_client = it->second;
			return ctl;		
		}
		return ctl;
	}
	bool ioChannel::insert(const std::string& addr, std::shared_ptr<coClient> cli)
	{
		if (auto it = m_clients.find(addr); it != m_clients.end())
		{			
			auto c = it->second;			
			if (c->valid())
			{				
				c->_close();				
			}			
			cli->safe_append(*c);			
			it->second = cli;
		}
		else
		{
			m_clients.emplace(addr, cli);
		}
		return true;
	}
	std::shared_ptr<coClient> ioChannel::find(const std::string& addr)
	{
		if (auto it = m_clients.find(addr); it != m_clients.end())
		{
			return it->second;
		}		
		return { nullptr };
	}
	int ioChannel::on_init(const std::string& addr, std::shared_ptr<coClient> c)
	{
		insert(addr, c);
		c->m_addr = addr;
		c->_send();
		
		return 0;
	}
	bool ioChannel::exist(const std::string& addr)
	{
		return m_clients.find(addr) != m_clients.end();
	}
	void ioChannel::erase(const std::string& addr)
	{
		m_clients.erase(addr);
	}
}
#include <exception>
#include "ioControl.h"
#include "ioChannel.h"

namespace ashan
{
	std::shared_ptr<coClient> ioControl::get()
	{
		auto c = m_client->valid() ? m_client : IOCH.find(m_client->m_addr);
		if (c)
			return c;
		throw "can't find ioControl";		
	}
	ioControl::ioControl(ioControl&& o)
	{		
		std::swap(m_client, o.m_client);
	}
	ioControl::ioControl(std::shared_ptr<coClient> c)
	{		
		m_client = c;
	}
	ioControl::ioControl(const ioControl& o) :m_client(o.m_client)
	{
	}
	std::shared_ptr<coClient> ioControl::operator->()
	{
		return get();
	}
}
#ifndef IO_CHANNEL_H
#define IO_CHANNEL_H
#include "evcpp.h"
#include "coClient.h"
#include "ioControl.h"

#define IOCH ioChannel::ins()

namespace ashan
{	
	class ioChannel : public ThreadLocal<ioChannel>
	{
		std::map<std::string, std::shared_ptr<coClient>> m_clients;			
	public:
		std::string m_addr;

		ioControl get(const std::string& addr);

		std::shared_ptr<coClient> find(const std::string& addr);
				
		bool insert(const std::string& addr, std::shared_ptr<coClient> cli);		

		bool exist(const std::string& addr);
		
		void erase(const std::string& addr);		

		int on_init(const std::string& addr, std::shared_ptr<coClient> c);
	};
}
#endif
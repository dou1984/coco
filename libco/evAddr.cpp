#include <sstream>
#include "evAddr.h"
namespace ashan
{
	bool evAddr::search(const std::string& name)
	{
		std::string addr;
		std::string ip;
		int port;
		return search(name, addr, ip, port);
	}
	bool evAddr::search(const std::string& name, std::string& ip, int& port)
	{
		std::string addr;
		return search(name, addr, ip, port);
	}
	bool evAddr::search(const std::string& name, std::string& addr, std::string& ip, int& port)
	{
		std::smatch sm;
		if (std::regex_search(name, sm, e0))
		{
			addr = std::move(sm[0]);
			ip = std::move(sm[1]);
			port = std::stoi(sm[2]);
			return true;
		}
		if (std::regex_search(name, sm, e1))
		{
			ip = "0.0.0.0";
			port = std::stoi(sm[1]);
			addr = (std::string)ip + ":" + std::to_string(port);
			return true;
		}
		return false;
	}
	bool evAddr::splite(const std::string& str, std::string& name, std::string& addr)
	{
		std::smatch sm;
		if (std::regex_search(str, sm, ea))
		{
			name = std::move(sm[1]);
			addr = std::move(sm[2]);
			return true;
		}
		return  false;
	}
	std::string evAddr::combine(const std::string& ip, int port)
	{		
		return combine(ip.c_str(), port);
	}
	std::string evAddr::combine(const char* ip, int port)
	{
		std::ostringstream oss;
		oss << ip << ":" << port;
		return oss.str();
	}
}
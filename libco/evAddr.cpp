#include <sstream>
#include "evAddr.h"
namespace ashan
{
	bool evAddr::search(const std::string& name)
	{
		std::string exe;
		std::string ip;
		int port;
		return search(name, exe, ip, port);
	}
	bool evAddr::search(const std::string& name, std::string& ip, int& port)
	{
		std::string exe;
		return search(name, exe, ip, port);
	}
	bool evAddr::search(const std::string& name, std::string& exe, std::string& ip, int& port)
	{
		std::smatch sm;
		if (std::regex_search(name, sm, e0))
		{
			exe = std::move(sm[0]);
			ip = std::move(sm[1]);
			port = std::stoi(sm[2]);
			return true;
		}
		else if (std::regex_search(name, sm, e1))
		{
			ip = "0.0.0.0";
			port = std::stoi(sm[1]);
			exe = (std::string)ip + ":" + std::to_string(port);
			return true;
		}		
		return false;
	}
	bool evAddr::splite(const std::string& str, std::string& exe, std::string& addr)
	{
		std::smatch sm;
		if (std::regex_search(str, sm, ea))
		{
			exe = std::move(sm[1]);
			addr = std::move(sm[2]);
			return true;
		}	
		return  false;
	}
	bool evAddr::splite(const std::string& path, std::string& exe, std::string& ip, int& port)
	{
		std::smatch sm;
		if (std::regex_search(path, sm, eb))
		{
			exe = std::move(sm[1]);
			ip = std::move(sm[2]);
			port = std::stoi(sm[3]);
			return true;
		}
		else if (std::regex_search(path, sm, e0))
		{
			ip = std::move(sm[1]);
			port = std::stoi(sm[2]);
			return true;
		}
		else if (std::regex_search(path, sm, e1))
		{
			ip = "0.0.0.0";
			port = std::stoi(sm[1]);			
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
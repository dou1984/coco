#ifndef EVADDR_H
#define EVADDR_H
#include <string.h>
#include <regex>
#include <Singleton.h>

#define EVADDR Singleton<ashan::evAddr>::ins()

namespace ashan
{
	class evAddr
	{
		std::regex e0{ R"((\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}):(\d+))" };
		std::regex e1{ R"(:(\d+))" };
		std::regex ea{ R"((\S+)/(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}:\d+))" };
	public:
		evAddr() = default;

		bool search(const std::string& name);

		bool search(const std::string& name, std::string& ip, int& port);

		bool search(const std::string& name, std::string& addr, std::string& ip, int& port);

		bool splite(const std::string& str, std::string& name, std::string& addr);

		std::string combine(const std::string& ip, int port);

		std::string combine(const char* ip, int port);
	};
}
#endif

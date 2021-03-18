/*
 *	Copyright (c) 2019-2020 Zhao YunShan(93850592@qq.com)
 */
#ifndef EVSERVER_H__
#define EVSERVER_H__
#include "Common.h"
#include "VClose.h"

namespace ashan
{
	class evServer
	{
		int m_fd = INVALID;
		ev_io m_Accept;
		std::string m_Addr = "";
		static void _accept(struct ev_loop* loop, struct ev_io* w, int revents);
		int _start(const char* ip, int port);
	public:		
		int start(const char* ip, int port);
		int stop();
		virtual ~evServer();
		virtual int on_accept(int fd); 
	};
}
#endif

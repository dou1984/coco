/* *	Copyright (c) 2019-2020 Zhao YunShan(93850592@qq.com)
 */
#ifndef EVCONNECT_H__
#define EVCONNECT_H__
#include "Common.h"
#include "VClose.h"

namespace ashan
{
	class evConnect
	{
		ev_io m_Read;
		ev_io m_Write;		
		std::string m_Addr;
		uint32_t m_tag = 0;
		size_t m_verify = MAGIC_WORD;
		void _connect();
		static void _cb_connect(struct ev_loop* loop, struct ev_io* w, int revents);
	protected:
		int m_fd = INVALID;
	public:
		evConnect();
		virtual ~evConnect();
		int connect(const char* _ip, int _port);
		virtual int on_connect(int _fd, const char* _ip, int _port) { return 0; }
		virtual int err_connect(int _fd, const char* _ip, int _port) { return 0; }
	};
}
#endif
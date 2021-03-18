/*
 *	Copyright (c) 2019-2020 Zhao YunShan(93850592@qq.com)
 */
#ifndef EVCLIENT_H__
#define EVCLIENT_H__
#include "Common.h"
#include "evTimer.h"
#include "VClose.h"
#include "ioBuffer.h"

namespace ashan
{
	class evClient : public VClose
	{
		int m_fd = INVALID;
		ev_io m_Read;
		ev_io m_Write;
		VClose* m_Close = nullptr;
		uint32_t m_tag = 0;
		size_t m_verify = MAGIC_WORD;
		static void _cb_read(struct ev_loop* loop, struct ev_io* w, int revents);
		static void _cb_write(struct ev_loop* loop, struct ev_io* w, int revents);
	protected:
		std::list<ioBuffer<char>> m_WriteBuffer;
		ioBufferEx<char> m_ReadBuffer;				
	public:
		evClient(int fd, VClose* _Close);
		~evClient();
		int _send(ioBuffer<char>&);
		int _send(const char* buffer, int _size);
		int _send();
		virtual int on_recv();
		int on_close(int fd);
		int _close();
		bool valid();		
		int _clear();
	};
}
#endif

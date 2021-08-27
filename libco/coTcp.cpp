#include "Common.h"
#include "coTcp.h"
#include "coConnect.h"
#include "coServer.h"
#include "coSleep.h"
#include "ioChannel.h"
#include "coServer.h"

namespace ashan
{
	iobc sayhi()
	{
		iobc hi_msg;
		hi_msg.append([=](auto buffer, auto size) {
			coMsg* msg = new (buffer)coMsg(0, e_rpc_sys_msg_sayhi);
			return msg->push_back(IOCH.m_addr.c_str(), IOCH.m_addr.size() + 1);
			});
		return hi_msg;
	}
	awaiter<size_t> co_heartbeat(std::shared_ptr<coClient> c, double _t)
	{		
		CO_DBG("%s", "begin co_heartbeart");			
		auto response = [=]()->coTask
		{
			while (c->valid())
			{
				c->set_waittime(_t * 1000);
				co_await c->co_read(e_rpc_sys_msg_heardbeat);
			}
		};
		auto request = [=]()->coTask
		{
			while (c->valid())
			{
				iobc b;
				b.append([](auto buffer, auto size) {
					coMsg* msg = new (buffer) coMsg(0, e_rpc_sys_msg_heardbeat);
					return msg->size();
					});
				co_await c->co_write(b);
				co_await coSleep(_t);
			}
		};
		co_await coTask::wait_for(response(), request());
	}
	coTask co_dial(const std::string& _addr, coFunc&& _co_process)
	{
		std::string addr = _addr;
		coFunc co_process = std::move(_co_process);
		while(true)
		{
			coConnect conn;
			CO_DBG("connect %s", addr.c_str());
			auto c = co_await conn.co_connect(addr);
			if (c && c->valid())
			{
				CO_DBG("%s", "sayhi");
				auto b = sayhi();

				auto idx = co_await c->co_write(b);

				CO_DBG("write success %d", idx);

				auto r = co_await c->co_read(idx);

				auto msg = c->get_data();

				CO_DBG("%s", "co_heartbeat");
				co_heartbeat(c, 6.0);

				if (co_process)
				{
					co_process(c);
				}
				co_await c->co_close();
			}
			co_await coSleep(0.5);
		}
	}

	coTask co_echo(coServer& srv, const std::string& _addr, coFunc&& _co_process)
	{
		std::string addr = _addr;		
		srv.start(addr);	
		coFunc co_process = std::move(_co_process);
		while (true)
		{
			auto c = co_await srv.co_accept();
			
			auto r = co_await c->co_read(e_rpc_sys_msg_sayhi);
			if (IS_FAILED(r))			
				continue;

			auto msg = c->get_data();

			auto b = sayhi();

			co_await c->co_write(b);

			IOCH.on_init(msg->data, c);

			CO_DBG("co_heartbeat %s", msg->data);

			co_heartbeat(c, 6.0);

			co_process(c);			

		}
	}
}
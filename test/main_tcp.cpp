#include "main_tcp.h"
#include "coTcp.h"
#include "ioBuffer.h"
#include "ioControl.h"

using namespace ashan;

#define e_test (10)

coTask try_run(std::shared_ptr<coClient> c)
{	
	CO_DBG("%s", "try run begin");
	auto f1 = [=](std::shared_ptr<coClient> c) ->coTask
	{
		while (true)
		{
			iobc b;
			b.append([](auto buf, auto size) {
				coMsg* msg = new (buf)coMsg(0, e_test);
				return msg->push_back("hello", 5);
				});		
			auto r = co_await c->co_write(b);			
			if (IS_FAILED(r))
			{
				CO_DBG("write error %d", r);
				co_return;
			}
			r = co_await c->co_read(r);
			if (IS_FAILED(r))
			{
				CO_DBG("request error %d", r);
				co_return;
			}

			auto msg = c->get_data();
			std::string s(msg->data, msg->length);
		}
	};
	co_await coTask::wait_for_all(f1(c));
}

coTask try_echo(std::shared_ptr<coClient> c)
{	
	
	CO_DBG("%s", "try echo begin");
	auto f1 = [=](std::shared_ptr<coClient> c) ->coTask
	{		
		const int max_repeat_count = 100000000;
		for (int i = 0; i < max_repeat_count; ++i)
		{			
			auto r = co_await c->co_read(e_test);
			if (IS_FAILED(r))
				break;
			auto msg = c->get_data();
			std::string s(msg->data, msg->length);			

			iobc b;
			b.append([=](auto buf, auto size) {
				coMsg* _msg = new(buf)coMsg(0, e_test);
				return _msg->push_back("welcome", 8);
				});
			r = co_await c->co_write(b);
			if (IS_FAILED(r))
				break;

		}
		c->_close();	
	};

	co_await coTask::wait_for(f1(c));
}
awaiter<size_t> test_co_run(coConnect& conn)
{
	co_await coTask::wait_for(co_dial("0.0.0.0:8080", try_run));
}

int main_test_server()
{
	std::thread t0([=]() {

		coServer srv;		

		co_echo(srv, "0.0.0.0:8080", try_echo);

		evLoop::start();

		});

	t0.detach();

	return 0;
}


int main_test_client()
{
	std::thread f([=]() {
		IOCH.m_addr = "0.0.0.0:9999";		
		coConnect conn;	

		test_co_run(conn);

		evLoop::start();

		});

	f.detach();	
	return 0;
}



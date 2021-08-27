#include <co.h>
#include <thread>
#include "main_awaiter.h"

using namespace ashan;

coAwaiter co_awaiter_2()
{
	CO_DBG("%s", "co_awaiter_2 begin");
	co_await coSleep(2);
	CO_DBG("%s", "co_awaiter_2 end");
}

coAwaiter co_awaiter_1()
{
	CO_DBG("%s", "co_awaiter_1 begin");
	co_await co_awaiter_2();
	CO_DBG("%s", "co_awaiter_1 end");
}
awaiter<int> co_awaiter()
{
	CO_DBG("%s", "co_awaiter begin");
	co_await co_awaiter_1();
	CO_DBG("%s", "co_awaiter end");	
}
int main_awaiter()
{
	std::thread f([=]() {
		
		CO_DBG("%s", "main_awaiter begin");
		co_awaiter();
		CO_DBG("%s", "main_awaiter end");
		evLoop::start();

		});

	f.detach();
	return 0;	
}
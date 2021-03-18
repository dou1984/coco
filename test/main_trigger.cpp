#include "main_trigger.h"
#include "co.h"

using namespace ashan;

coTrigger<size_t> t;

const int _key = 100;
coTask go()
{
	CO_DBG("%s", "begin");
	co_await t.suspend(_key);
	CO_DBG("%s", "end");
}

coTask sleep()
{
	CO_DBG("%s", "begin");
	co_await coSleep(2.0);
	CO_DBG("%s", "end");
}
awaiter<size_t> test()
{

	go();

	co_await coTask::wait_for_all(sleep());

	t.resume(_key);

}

int main_test_trigger()
{
	
	test();

	evLoop::start();

	return 0;
}
#include "main_task.h"


awaiter<size_t> test_when()
{
	auto f1 = [=]() -> coTask
	{
		CO_DBG("event 1 %s", "begin");
		co_await coSleep(1.0);
		CO_DBG("event 1 %s", "end");
	};

	auto f2 = [=]() -> coTask
	{
		CO_DBG("event 2 %s", "begin");
		co_await coSleep(2.0);
		CO_DBG("event 2 %s", "again");
		co_await coSleep(2.0);
		CO_DBG("event 2 %s", "end");
	};


	CO_DBG("when %s", "begin");
	co_await coTask::wait_for_all(f1(), f2());
	CO_DBG("when %s ", "end");

}

awaiter<size_t> test_when2()
{
	auto f1 = [=]() -> coTask
	{
		CO_DBG("event 1 %s", "begin");
		co_await coSleep(1.0);
		CO_DBG("event 1 %s", "end");
	};

	auto f2 = [=]() -> coTask
	{
		CO_DBG("event 2 %s", "begin");
		co_await coSleep(2.0);
		CO_DBG("event 2 %s", "end");
	};

	CO_DBG("when %s", "begin");
	co_await coTask::wait_for(f1(), f2());
	CO_DBG("when %s ", "end");
}

void test_when_main()
{
	test_when();

	test_when2();

	evLoop::start();
}
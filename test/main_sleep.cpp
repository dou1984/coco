#include "main_sleep.h"


coTask test_sleep(double t)
{
	CO_DBG("sleep begin %f", t);
	co_await coSleep(t);
	CO_DBG("sleep end %f", t);
}

awaiter<size_t> test_sleep_1()
{
	co_await coTask::wait_for(
		test_sleep(1),
		test_sleep(2),
		test_sleep(3)
		);
	CO_DBG("%s", "wait end");
}
awaiter<size_t> test_sleep_2()
{
	co_await coTask::wait_for(
		test_sleep(1),
		test_sleep(1),
		test_sleep(1)
	);
	CO_DBG("%s", "wait end");
}
awaiter<size_t> test_sleep_3()
{
	co_await coTask::wait_for_all(
		test_sleep(1),
		test_sleep(2),
		test_sleep(3)
	);
	CO_DBG("%s", "wait end");
}

int test_sleep_main()
{
	test_sleep_1();
	test_sleep_2();
	test_sleep_3();

	evLoop::start();
	return 0;
}
#include "main_mutex.h"

awaiter<> mtx_l(coMutex& m)
{
	CO_DBG("lock %s", "begin");
	co_await m.lock();
	CO_DBG("lock %s", "end");
}
void mtx_ul(coMutex& m)
{
	CO_DBG("unlock %s", "begin");
	m.unlock();
	CO_DBG("unlock %s", "end");

}
int test_mutex()
{
	coMutex m;
	mtx_l(m);
	mtx_l(m);
	mtx_ul(m);
	mtx_ul(m);
	return 0;
}

coMutex g_mtx;
coTask test_auto_mutex_(bool co_sleep = true)
{
	auto l = co_await g_mtx.auto_lock();

	CO_DBG("%s", "begin");
	if (co_sleep)
		co_await coSleep(2.0);
	else
		std::this_thread::sleep_for(std::chrono::seconds(2));
	CO_DBG("%s", "end");
}
awaiter<size_t> test_wait_for_mutex()
{
	co_await coTask::wait_for(test_auto_mutex_());
	CO_DBG("%s", "tam end");
}
int twfm()
{
	test_wait_for_mutex();
	evLoop::start();
	return 0;
}
int test_auto_mutex()
{
	std::thread t0(twfm);
	t0.detach();
	std::thread t1(twfm);
	t1.detach();
	std::thread t2(twfm);
	t2.detach();
	std::thread t3(twfm);
	t3.detach();

	return 0;
}
awaiter<size_t>  test_mutex_xxx()
{
	co_await coTask::wait_for(
		test_auto_mutex_(),
		test_auto_mutex_(),
		test_auto_mutex_());

	CO_DBG("%s", "wait end");
	co_return 0;
}
int test_mutex_main()
{
	test_mutex_xxx();
	evLoop::start();
	return 0;
}
int test_mutex_thread()
{
	std::thread t0(test_mutex_main);

	t0.detach();

	return 0;

}
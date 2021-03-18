#include "main_channel.h"

coTask ch_r(coChannel<int>& ch)
{
	int x = 0;
	CO_DBG("%d begin", x);
	co_await ch.read(x);
	CO_DBG("%d end", x);
}
coTask ch_w(coChannel<int>& ch, int y, double t = 1.0)
{
	//int y = 100;
	CO_DBG("%s", "begin");
	co_await coSleep(t);
	CO_DBG("%s", "timeover");

	co_await ch.write(y);
	CO_DBG("%d end", y);
}

coTask ch_aw(coChannel<int>& ch, int y)
{
	//int y = 100;
	CO_DBG("%s", "begin");	
	co_await ch.async_write(y);
	CO_DBG("%d end", y);
}
awaiter<size_t> test_channel_1()
{
	coChannel<int> ch;

	auto r1 = ch_r(ch);
	ch_w(ch, 100);
	ch_w(ch, 200);
	ch_w(ch, 300);

	auto r2 = ch_r(ch);
	auto r3 = ch_r(ch);

	CO_DBG("%s", "wait for all begin");
	co_await coTask::wait_for_all(r1, r2, r3);
	CO_DBG("%s", "wait for all end");

}

awaiter<size_t> test_channel_2()
{
	coChannel<int> ch;

	auto r1 = ch_r(ch);
	auto r2 = ch_w(ch, 100);
	
	auto r3 = ch_r(ch);
	auto r4 = ch_r(ch);

	auto r5 = ch_w(ch, 200, 2.0);
	auto r6 = ch_w(ch, 300);

	co_await coTask::wait_for(r1, r2, r3, r4, r5, r6);
	CO_DBG("%s", "wait for end");

}

awaiter<size_t> test_channel_3()
{
	coChannel<int> ch;

	auto r1 = ch_r(ch);
	ch_aw(ch, 100);
	ch_aw(ch, 200);
	ch_aw(ch, 300);
	auto r2 = ch_r(ch);
	auto r3 = ch_r(ch);
	co_await coTask::wait_for_all(r1, r2, r3);
	CO_DBG("%s", "wait for end ");
}
awaiter<size_t> test_channel_4()
{
	coChannel<int> ch;

	auto r1 = ch_r(ch);
	ch_aw(ch, 100);
	ch_aw(ch, 200);
	ch_aw(ch, 300);
	auto r2 = ch_r(ch);
	auto r3 = ch_r(ch);

	co_await coTask::wait_for(r1, r2, r3);
	CO_DBG("%s", "wait for end ");
}
int test_channel_main()
{
	test_channel_1();
	test_channel_2();
	test_channel_3();
	test_channel_4();

	evLoop::start();
	return 0;
}
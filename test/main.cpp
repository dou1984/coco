#include "main_channel.h"
#include "main_mutex.h"
#include "main_task.h"
#include "main_sleep.h"
#include "main_tcp.h"
#include "main_trigger.h"
#include "main_awaiter.h"

using namespace ashan;


int main(int argc, char** argv)
{
	//test_sleep_main();
	//test_channel_main();
	//test_auto_mutex();
	//test_mutex_thread();
	
	/*
 	if (argc == 1)
 		main_test_server();
 	else
 		main_test_client();
		*/
	//main_test_trigger();

	main_awaiter();

	getchar();

	return 0;
}

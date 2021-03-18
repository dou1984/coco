#ifndef CO_TCP_H
#define CO_TCP_H
#include "coTask.h"
#include "ioBuffer.h"
#include "coClient.h"
#include "coServer.h"

namespace ashan
{	
	using iobc = ioBuffer<char>;
	
	iobc sayhi();

	awaiter<size_t> co_heartbeat(std::shared_ptr<coClient> c, double _t);

	coTask co_dial(const std::string& addr, coFunc&& co_process);

	coTask co_echo(coServer& srv, const std::string& addr, coFunc&& co_process);
}

#endif

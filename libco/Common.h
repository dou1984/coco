/*
 *	Copyright (c) 2019-2020 Zhao YunShan(93850592@qq.com)
 */
#ifndef COMMON_H_
#define COMMON_H_
#include <map>
#include <memory>
#include <bitset>
#include <array>
#include <list>
#include <string.h>
#include <assert.h>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <functional>
#include <coroutine>
#include <algorithm>
#include <chrono>
#include <atomic>
#include <regex>
#include <set>
#include <ev++.h>
#include "ThreadLocal.h"

#define INVALID			(-1)
#define TIMEOUT			(15.0f)
#define LONGTIMEOUT		(150.0f)
#define RECONNTIME		(0.5f)
#define BUFFERMAX		(0x10000)
#define RPC_INVALID		(0)
#define MAGIC_WORD		0xfedcba9876543210
#define MAX_RPC_INDEX	(0xfffffff0)

#define CO_DBG(STR, ...) printf("%s:%s:%d|" STR "\n", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define IS_SUCCESS(IDX)	(IDX < e_rpc_msg_begin)
#define IS_FAILED(IDX)	(IDX >= e_rpc_msg_begin)
#define DELAY_RESUME

enum e_rpc_msg
{
	e_rpc_msg_begin = MAX_RPC_INDEX,	
	e_rpc_msg_sayhi,
	e_rpc_msg_heardbeat,
	e_rpc_msg_ack,		
	e_rpc_msg_timeout = 0xfffffffe,
	e_rpc_msg_close = 0xffffffff,
};

#endif

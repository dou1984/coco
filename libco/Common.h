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
#include <string>
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
#include <ev.h>
#include "ThreadLocal.h"

#define INVALID			(-1)
#define TIMEOUT			(15.0f)
#define LONGTIMEOUT		(150.0f)
#define RECONNTIME		(0.5f)
#define BUFFERMAX		(0x10000)
#define RPC_INVALID		(0)
#define MAGIC_WORD		0xfedcba9876543210
#define MAX_RPC_INDEX	(0xff00)
#define MIN_ERROR_INDEX (0xfff0)
#define MAX_SYS_INDEX	(0xffff)

#define CO_DBG(STR, ...) printf("[%s:%s:%d]" STR "\n", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define IS_SUCCESS(IDX)	(!IS_FAILED(IDX))
#define IS_FAILED(IDX)	((IDX >> 4) == 0xfff)
#define IS_CUSTOM(IDX)	(IDX <= MAX_RPC_INDEX)
#define IS_RPCMSG(IDX)	(IDX > MAX_SYS_INDEX)

#define DELAY_RESUME

enum e_rpc_msg
{
	e_rpc_sys_msg_begin = MAX_RPC_INDEX,
	e_rpc_sys_msg_heardbeat = e_rpc_sys_msg_begin,
	e_rpc_sys_msg_sayhi,	
	e_rpc_sys_msg_ack,
	e_rpc_sys_msg_custom,
	e_rpc_sys_msg_error = MIN_ERROR_INDEX,
	e_rpc_sys_msg_timeout,
	e_rpc_sys_msg_close,
};

#endif

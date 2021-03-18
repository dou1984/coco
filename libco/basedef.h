#ifndef __BASEDEF_H__
#define __BASEDEF_H__
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <limits>

typedef int	SOCKET;
typedef int BOOL;

#define NAME_BUFFER_MAX  (24)
#define KILO * 1024

#define INVALID (-1)
#define IPBUFFERSIZE (16)
#define STRCPY(DST, SRC) do { strncpy(DST, SRC, sizeof(DST) - 1); DST[sizeof(DST) - 1] = '\0'; } while (0)

#define TRUE (1)
#define FALSE (0)
#define COMPRESS_SIZE_MIN (4 KILO)
#define HEARTBEAT_TIME (6 * 1000)
#define RECONNECT_TIME (6 * 1000)
#define BUFFER_WARNNING_SIZE (60 KILO)

inline void SIN_ZERO(void* sin_zero)
{
	unsigned long long* ptr = (unsigned long long*)sin_zero;
	*ptr = 0;
}

#if defined(__ANDROID__) || defined(__APPLE__) || defined(GDBTEST)
#define THREADSAFE
#endif

const char* VERSION();

#define _THROW(EXPR, INFO) if (!(EXPR)) throw(INFO)

#endif
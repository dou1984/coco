#ifndef __BASE_SOCKET_H__
#define __BASE_SOCKET_H__
#include <sys/socket.h>
#ifndef __ANDROID__
#include <sys/fcntl.h>
#else
#include <fcntl.h>
#endif
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#ifdef __ANDROID__
#include <sys/epoll.h>  
#elif __APPLE__
#include <sys/event.h>
#include <sys/time.h>
#include <sys/syscall.h>
#else
#include <sys/epoll.h>  
#include <sys/timerfd.h>  
#endif
#include "basedef.h"


inline int32_t BindLocal(SOCKET fd, const char* ip)
{
	struct sockaddr_un addr;
	addr.sun_family = AF_LOCAL;	
	addr.sun_path[0] = '\0';
	strcpy(addr.sun_path + 1, ip);
	return ::bind(fd, (sockaddr*)&addr, sizeof(addr.sun_family) + strlen(ip) + 1);
}
inline int32_t Bind(SOCKET fd, const char* ip, int32_t port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);	
	inet_pton(addr.sin_family, ip, &addr.sin_addr);
	SIN_ZERO(addr.sin_zero);
	return ::bind(fd, (sockaddr*)&addr, (int)sizeof(addr));
}
inline int32_t ConnectLocal(SOCKET fd, const char* ip)
{
	sockaddr_un addr;
	addr.sun_family = AF_LOCAL;	
	addr.sun_path[0] = '\0';
	strcpy(addr.sun_path + 1, ip);	
	return ::connect(fd, (sockaddr*)&addr, sizeof(addr.sun_family) + strlen(ip) + 1);
}
inline int32_t Connect(SOCKET fd, const char* ip, int32_t port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);	
	inet_pton(addr.sin_family, ip, &addr.sin_addr);
	SIN_ZERO(addr.sin_zero);
    return ::connect(fd, (sockaddr*)&addr, sizeof(addr));
}
inline int GetSocketError(SOCKET fd)
{
	int error_value = 0;
	socklen_t len = sizeof(socklen_t);
	return getsockopt(fd, SOL_SOCKET, SO_ERROR, (char*)&error_value, &len) == -1 ? -1 : error_value;
}
inline SOCKET Accept(SOCKET fd, char* ip, int len, int& port)
{    
	sockaddr_in addr;
	socklen_t addr_len = sizeof(sockaddr_in);
	SOCKET rfd = accept(fd, (sockaddr*)&addr, &addr_len);
    if (rfd != INVALID)
    {
        inet_ntop(AF_INET, &(addr.sin_addr), ip, len);        
        port = ntohs(addr.sin_port);
    }
    else
    {
        strncpy(ip, "", len);
        port = 0;
    }
    return rfd;
}
inline SOCKET AcceptLocal(SOCKET fd, char* ip, int len)
{
	sockaddr_un addr;
	addr.sun_family = AF_LOCAL;
	addr.sun_path[0] = '\0';	
	socklen_t addr_len = sizeof(sockaddr_un);
	SOCKET rfd = accept(fd, (sockaddr*)&addr, &addr_len);
	if (rfd != INVALID)
	{
		strncpy(ip, addr.sun_path, len);
		ip[len - 1] = '\0';		
	}	
	return rfd;
}
inline int32_t SetNoBlock(SOCKET fd, bool block)
{
	int32_t ret = 0;
	if (block)
	{
		ret = fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL));
	}
	else
	{
		ret = fcntl(fd, F_SETFL, ~O_NONBLOCK & fcntl(fd, F_GETFL));
	}
	return ret;
}
inline int32_t SetKeepAlive(SOCKET fd, int alive)
{	
	return setsockopt(fd, IPPROTO_TCP, SO_KEEPALIVE, (char*)&alive, sizeof(alive));
}

inline int32_t SetTTL(SOCKET fd, int32_t ttl)
{
    return setsockopt(fd, IPPROTO_IP, IP_TTL, (char *)&ttl, sizeof(ttl));
}
inline int32_t GetTTL(SOCKET fd, int32_t* ttl)
{
    socklen_t len = sizeof(int32_t);
    return getsockopt(fd, IPPROTO_IP, IP_TTL, ttl, &len);
}
inline int32_t SetTOS(SOCKET fd, int32_t tos)
{
    return setsockopt(fd, IPPROTO_IP, IP_TOS, (char *)&tos, sizeof(tos));
}
inline int32_t GetTOS(SOCKET fd, int32_t* tos)
{
    socklen_t len = sizeof(int32_t);
    return getsockopt(fd, IPPROTO_IP, IP_TOS, tos, &len);
}
inline int32_t SetReuseAddr(SOCKET fd, int reuse)
{
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
}
inline int32_t SetSendTimeOut(SOCKET fd, int32_t _timeout)
{
    struct timeval timeout = {_timeout, 0};
	socklen_t len = sizeof(timeout);
	return setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, len);
}
inline int32_t SetRecvTimeOut(SOCKET fd, int32_t _timeout)
{
    struct timeval timeout = {_timeout, 0};
	socklen_t len = sizeof(timeout);
	return setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, len);
}
inline int32_t SetSendBuffSize(SOCKET fd, int32_t bufsize)
{
    return setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
}
inline int32_t SetRecvBuffSize(SOCKET fd, int32_t bufsize)
{
    return setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize));
}
inline int32_t GetSendBuffSize(SOCKET fd, int32_t* bufsize)
{
    socklen_t len = sizeof(int32_t);
    return getsockopt(fd, SOL_SOCKET, SO_SNDBUF, bufsize, &len);
}
inline int32_t GetRecvBuffSize(SOCKET fd, int32_t* bufsize)
{
    socklen_t len = sizeof(int32_t);
    return getsockopt(fd, SOL_SOCKET, SO_RCVBUF, bufsize, &len);
}
inline int32_t SetLingerOff(SOCKET fd)
{
    struct linger so_linger;
    so_linger.l_onoff = 0;
    so_linger.l_linger = 0;
    return setsockopt(fd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger));
}
inline int32_t SetLingerOn(SOCKET fd, uint16_t _timeout)
{
    struct linger so_linger;
	so_linger.l_onoff = _timeout == 0 ? 0 : 1;
    so_linger.l_linger = _timeout;
    return setsockopt(fd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger));
}
inline int32_t SetNoDelay(SOCKET fd, int nodelay)
{
    return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay, sizeof(nodelay));
}
inline int32_t GetSockName(SOCKET fd, char* ip, int len, int32_t* port)
{
    if (ip == NULL || port == NULL) {
        return INVALID;
    }
    char host_ip[32];
	struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int ret = ::getsockname(fd, (struct sockaddr *)&addr, &addr_len);
    if (ret == 0)
	{
        inet_ntop(AF_INET, &addr.sin_addr, host_ip, sizeof(host_ip));
        strncpy(ip, host_ip, len);
        *port = ntohs(addr.sin_port);
    }
    else
    {
        strncpy(ip, "", len);
        *port = 0;
    }
    return ret;
}

inline int32_t GetPeerName(SOCKET fd, char* ip, int len, int32_t& port)
{    
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
    int ret = ::getpeername(fd, (struct sockaddr *)&addr, &addr_len);
    if (ret == 0)
	{
		inet_ntop(AF_INET, &addr.sin_addr, ip, len);
        port = ntohs(addr.sin_port);
    }
    else
    {
        strncpy(ip, "", len);
        port = 0;
    }
    return ret;
}
inline int32_t GetHostByName(const char* name, char* ip, int len)
{    
    struct hostent * h = NULL;
	struct in_addr addr;	
	h = gethostbyname(name);    
    if (!h) {
        return INVALID;
    }
    addr = *(struct in_addr*) h->h_addr_list[0];
    inet_ntop(AF_INET, &addr, ip, len);    
    return 0;
}
#endif

/*
 *	Copyright (c) 2019-2020 Zhao YunShan(93850592@qq.com)
 */
#ifndef VCLOSE_H__
#define VCLOSE_H__

class VClose
{
public:
	virtual ~VClose() = default;
	virtual int on_close(int fd) = 0;	
};

#endif
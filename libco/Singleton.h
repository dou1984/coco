/*
 *	Copyright (c) 2019-2020 Zhao YunShan(93850592@qq.com)
 *	All rights reserved.
 *	单例模型
 *
 */
#ifndef __SINGLETON_H__
#define __SINGLETON_H__

template<class _Type>
class Singleton {
public:
	static _Type& ins(){		
		static _Type obj;
		return obj;
	}	
};
#endif

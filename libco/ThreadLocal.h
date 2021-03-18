#ifndef THREADLOCAL_H
#define THREADLOCAL_H

namespace ashan
{
	template<class _Type>
	struct ThreadLocal
	{
		static _Type& ins() {
			static thread_local _Type obj;
			return obj;
		}
	};
}
#endif
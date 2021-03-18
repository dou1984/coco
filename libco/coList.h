#ifndef CO_LIST_H
#define CO_LIST_H
#include <stddef.h>
#include <algorithm>
#include <cassert>

namespace ashan
{
	class coList
	{
		coList* m_prev = this;
		coList* m_next = this;
	public:		
		coList() = default;
		coList(coList&&) = delete;
		coList(const coList&) = delete;
		void push_front(coList* new_) { __list_add(new_, this, m_next); }
		void push_back(coList* new_) { __list_add(new_, m_prev, this); }
		void erase(coList* old) { __list_del(old->m_prev, old->m_next);	old->m_prev = old->m_next = old; }
		bool empty()const { return m_prev == this && m_next == this; }
		coList* front() { return m_next; }
		coList* back() { return m_prev; }		
		coList* begin() { return m_next; }
		coList* end() { return this; }
		void append(coList& o);

	private:
		void __list_add(coList* cur, coList* prev, coList* next);	
		void __list_del(coList* prev, coList* next);		
	};
}
#endif


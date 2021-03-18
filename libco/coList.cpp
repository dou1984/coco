#include "coList.h"

namespace ashan
{	
	void coList::append(coList& o)
	{		
		while (!o.empty())
		{
			auto it = o.front();
			o.erase(it);
			push_back(it);
		}		
	}	
	void coList::__list_add(coList* cur, coList* prev, coList* next)
	{		
		prev->m_next = next->m_prev = cur;
		cur->m_next = next;
		cur->m_prev = prev;
	}
	void coList::__list_del(coList* prev, coList* next)
	{
		next->m_prev = prev;
		prev->m_next = next;
	}
}
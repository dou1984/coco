/*
 *	Copyright (c) 2019-2020 Zhao YunShan(93850592@qq.com)
 *	All rights reserved. 
 */
#ifndef __IO_BUFFER_H__
#define __IO_BUFFER_H__
#include <cassert>

namespace ashan
{
	template<class _Type, size_t _Size = 0x1000>
	class ioBuffer
	{
	public:
		ioBuffer(int _size = _Size) : m_max(_size), m_size(0)
		{
			m_buffer = new _Type[m_max];
		}
		ioBuffer(const ioBuffer& other) = delete;
		ioBuffer(ioBuffer&& o)
		{
			*this = std::move(o);
		}
		const ioBuffer& operator=(ioBuffer&& o)
		{
			std::swap(m_max, o.m_max);
			std::swap(m_size, o.m_size);
			std::swap(m_buffer, o.m_buffer);
			return *this;
		}
		~ioBuffer()
		{
			if (m_buffer)
				delete[] m_buffer;
		}
		void push_back(const _Type& other)
		{
			if (m_size < m_max)
			{
				m_buffer[m_size++] = other;
				return;
			}
			reserve(m_max * 2);
			m_buffer[m_size++] = other;
		}
		void push_back(const _Type* other, size_t size)
		{
			if (m_size + size <= m_max)
			{
				memcpy(m_buffer + m_size, other, sizeof(_Type) * size);
				m_size += size;
				return;
			}
			size_t dmax = m_max > size ? m_max : size;
			reserve(dmax * 2);
			memcpy(m_buffer + m_size, other, sizeof(_Type) * size);
			m_size += size;
		}
		void emplace(size_t size)
		{
			if (m_size + size > m_max)
			{
				size_t dmax = m_max > size ? m_max : size;
				reserve(dmax * 2);
			}
		}
		template<class... _Args>
		void emplace_back(_Args&&... args)
		{
			if (m_size < m_max)
			{
				new (m_buffer + m_size++) _Type(std::forward<_Args>(args)...);
				return;
			}
			reserve(m_max * 2);
			new (m_buffer + m_size++) _Type(std::forward<_Args>(args)...);
		}
		template<class _F>
		auto append(const _F& _f, size_t _s = 0)
		{
			emplace(_s);
			auto r = _f(data() + size(), max() - size());			
			if (r > 0)
				resize(size() + r);
			return r;
		}
		void pop_front(size_t size)
		{
			if (size == m_size)
			{
				m_size = 0;
			}
			else if (size < m_size)
			{
				m_size -= size;
				memmove(m_buffer, m_buffer + size, sizeof(_Type) * m_size);
			}
			else
			{
				throw("pop_front size is error");
			}
		}
		_Type& operator[](size_t pos)
		{
			if (pos < m_size)
				return m_buffer[pos];
			throw("beyond max size!");
		}
		_Type& operator[](size_t pos)const
		{
			if (pos < m_size)
				return m_buffer[pos];
			throw("beyond max size!");
		}
		ioBuffer& operator=(const ioBuffer& other)
		{
			m_size = 0;
			if (max() < other.size())
				reserve(other.max());
			m_size = other.m_size;
			memcpy(m_buffer, other.m_buffer, sizeof(_Type) * m_size);
			return *this;
		}
		void reserve(size_t _max)
		{
			_Type* tmp_buffer = m_buffer;
			assert(_max >= m_max);
			m_max = _max;
			m_buffer = new _Type[m_max];
			assert(m_size <= m_max);
			if (m_size != 0)
				memcpy(m_buffer, tmp_buffer, sizeof(_Type) * m_size);
			delete[] tmp_buffer;
		}
		void resize(size_t _size)
		{
			if (_size > max())
				reserve(_size);
			m_size = _size;
		}
		void clear()
		{
			m_size = 0;
		}
		bool empty()const { return m_size == 0; }
		size_t max()const { return m_max; }
		size_t size()const { return m_size; }
		_Type* begin() const { return m_buffer; }
		_Type* end() const { return m_buffer + m_size; }
		_Type* front() { return m_buffer; }
		_Type* back()
		{
			if (m_size == 0)
				return end();
			return end() - 1;
		}
		_Type* back() const
		{
			if (m_size == 0)
				return end();
			return end() - 1;
		}
		_Type* data()const { return m_buffer; }
	private:
		size_t m_max = _Size;
		size_t m_size = 0;
		_Type* m_buffer = nullptr;
	};

	template<class _Type>
	class ioBufferEx : ioBuffer<_Type>
	{
		size_t m_Pos = 0;
		using base = ioBuffer<_Type>;
	public:
		ioBufferEx() = default;
		ioBufferEx(ioBufferEx&& o) : base(std::move(o))
		{
			std::swap(m_Pos, o.m_Pos);
		}
		const ioBufferEx& operator=(ioBufferEx&& o)
		{
			base* _base = this;
			*_base = std::move(o);
			std::swap(m_Pos, o.m_Pos);
			return *this;
		}
		_Type* data()const { return base::data() + m_Pos; }
		size_t size() { return base::size() - m_Pos; }
		void push_back(const _Type* other, size_t iSize)
		{
			if (base::size() + iSize >= base::max())
			{
				if (m_Pos > 0)
				{
					base::pop_front(m_Pos);
					m_Pos = 0;
				}
			}
			return base::push_back(other, iSize);
		}
		void emplace(size_t iSize)
		{
			if (base::size() + iSize >= base::max())
			{
				if (m_Pos > 0)
				{
					base::pop_front(m_Pos);
					m_Pos = 0;
				}
			}
			base::emplace(iSize);
		}
		void pop_front(size_t iSize)
		{
			m_Pos += iSize;
			if (m_Pos >= base::size())
			{
				base::pop_front(base::size());
				m_Pos = 0;
			}
		}
		bool empty()
		{
			if (base::empty())
				return true;
			if (m_Pos < base::size())
				return false;
			base::pop_front(base::size());
			m_Pos = 0;
			return true;
		}
		_Type* at(size_t pos)const
		{
			if (m_Pos + pos < base::size())
				return base::data() + m_Pos + pos;
			throw("at error pos");
		}
		size_t max()const
		{
			assert(m_Pos <= base::max());
			return base::max() - m_Pos;
		}
		void reserve(size_t c)
		{
			if (c < max())
				return;
			base::reserve(m_Pos + c);
		}
		_Type* end() { return base::data() + base::size(); }
		void clear() { pop_front(size()); }		
		using base::append;

	};
}

#endif

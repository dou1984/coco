#ifndef CO_MESSAGE_H
#define CO_MESSAGE_H
#include <stdint.h>
#include <string.h>

namespace ashan
{
#pragma pack(push, 1)
	struct coMsg
	{
		uint32_t length;
		union
		{
			uint32_t index;
			struct { uint16_t cmd; uint16_t reserve; };
		};
		char data[];
		coMsg() = default;
		coMsg(uint32_t l, uint32_t idx = 0) : length(l), index(idx)
		{
		}
		size_t size() const { return sizeof(coMsg) + length; }
		char* ptr() { return (char*)this; }
		size_t push_back(const char* _buf, int _size)
		{
			length += _size;
			memcpy(data, _buf, _size);
			return size();
		}
		void clear()
		{
			length = index = 0;
		}
	};
#pragma pack(pop)
}

#endif
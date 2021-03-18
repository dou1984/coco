#ifndef IO_CONTROL_H
#define IO_CONTROL_H
#include <memory>

namespace ashan
{
	class coClient;
	class ioControl
	{		
		std::shared_ptr<coClient> m_client;		
		std::shared_ptr<coClient> get();	
		friend class ioChannel;
	public:
		ioControl() = default;
		
		ioControl(std::shared_ptr<coClient> c);

		ioControl(const ioControl& o);

		ioControl(ioControl&& o);

		std::shared_ptr<coClient> operator->();
	};
}
#endif
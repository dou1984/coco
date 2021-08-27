#ifndef CO_FUNC_H
#define CO_FUNC_H
#include <functional>
#include <memory>

namespace ashan
{	
	class coClient;
	using coFunc = std::function<coTask(std::shared_ptr<coClient>)>;
}
#endif
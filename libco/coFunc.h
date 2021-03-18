#ifndef CO_FUNC_H
#define CO_FUNC_H
#include <functional>
#include "ioControl.h"

namespace ashan
{	
	using coFunc = std::function<coTask(ioControl)>;
}
#endif
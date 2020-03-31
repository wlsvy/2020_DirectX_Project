#pragma once
#include <string>
#include "../../Util/UnCopyable.h"

class BufferBase : public Uncopyable {
protected:
	std::string Name;
};
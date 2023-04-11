#pragma once 

#include "Stellar/Core/Core.h"
#include <string>

namespace Stellar {
	class STLR_API FileDialogs {
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}
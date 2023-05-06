#pragma once

#include "Stellar/Core/Core.h"

#include <string>
#include <ctime>

namespace Stellar {

	enum class ConsoleMessageFlags : int16_t {
		None = -1,
		Info = BIT(0),
		Warning = BIT(1),
		Error = BIT(2),
		Debug = BIT(3),
		Trace = BIT(4),

		All = Info | Warning | Error | Debug | Trace
	};

	struct ConsoleMessage {
		std::string ShortMessage;
		std::string LongMessage;
		int16_t Flags;

		time_t Time;
	};
	
}
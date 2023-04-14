#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/STLRBase.h"
#include "RendererAPI.h"

namespace Stellar {
	class STLR_API RendererContext : public STLR_Base {
	public:
		virtual ~RendererContext() = default;

		static STLR_Ptr<RendererContext> Create();

		[[nodiscard]] virtual std::string getGPUInfo() const = 0;
		[[nodiscard]] virtual std::string getGraphicsAPI() const = 0;
	};
}
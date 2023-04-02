#pragma once

#include "Stellar/Core/Core.h"
#include "RendererAPI.h"

namespace Stellar {
    class STLR_API RendererContext {
    public:
        virtual ~RendererContext() = default;

        static Ref<RendererContext> Create();

        [[nodiscard]] virtual std::string getGPUInfo() const = 0;
        [[nodiscard]] virtual std::string getGraphicsAPI() const = 0;
    };
}
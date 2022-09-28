#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/RendererContext.h"

namespace Stellar {
    class STLR_API MetalRendererContext : public RendererContext {
    public:
        MetalRendererContext();
        ~MetalRendererContext() override;

        [[nodiscard]] std::string getGPUInfo() const override;
        [[nodiscard]] std::string getGraphicsAPI() const override;
    };
}
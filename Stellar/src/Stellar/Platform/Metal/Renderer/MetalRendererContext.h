#pragma once

#include "Stellar/Core.h"
#include "Stellar/Renderer/RendererContext.h"

namespace Stellar {
    class STLR_API MetalRendererContext : public RendererContext {
        MetalRendererContext();
        ~MetalRendererContext() override;

        [[nodiscard]] std::string getGPUInfo() const override;
        [[nodiscard]] std::string getGraphicsAPI() const override;
    };
}
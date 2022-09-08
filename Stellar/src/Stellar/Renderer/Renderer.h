#pragma once

#include "Stellar/Core.h"

namespace Stellar {
    enum class RendererAPI {
        None = 0, Vulkan = 1, Metal = 2
    };

    class STLR_API Renderer {
    public:
        inline static RendererAPI GetAPI() { return s_RendererAPI; }
    private:
        static RendererAPI s_RendererAPI;
    };
}
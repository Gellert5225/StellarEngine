#pragma once

#include "Stellar/Core.h"
#include "RendererAPI.h"

namespace Stellar {

    class STLR_API Renderer {
    public:
        static void Init();
        static void Shutdown();

        static void BeginRenderPass();
        static void EndRenderPass();

    };
}
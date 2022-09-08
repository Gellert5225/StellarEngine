#pragma once

namespace Stellar {
    enum class RendererAPIType {
        None = 0, Vulkan = 1, Metal = 2
    };

    class RendererAPI {
    public:
        virtual void init() = 0;
        virtual void shutDown() = 0;
        virtual void beginRenderPass() = 0;
        virtual void endRenderPass() = 0;

        static RendererAPIType Current() { return s_CurrentRendererAPI; }
    private:
        inline static RendererAPIType s_CurrentRendererAPI = RendererAPIType::Vulkan;

    };
}
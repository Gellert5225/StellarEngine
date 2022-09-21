#pragma once

#include "Stellar/Core.h"

#include "Stellar/Renderer/RendererAPI.h"

#include "Stellar/Platform/Metal/MetalAPI.h"
#include "Stellar/Platform/Metal/Pipeline/MetalPipeline.h"

namespace Stellar {
    class STLR_API MetalRenderer : public RendererAPI {
    public:
        void init() override;
        void shutDown() override;

        void beginScene(Camera camera) override;
        void endScene() override;

        void beginRenderPass() override;
        void endRenderPass() override;

        void setClearColor(const glm::vec4& color) override;
        void renderGeometry(Buffer* vertexBuffer,
                            Buffer* indexBuffer,
                            uint32_t indexCount = 0,
                            const glm::mat4& transform = {}) override;

    private:
        MTL::ClearColor m_ClearColor = {0.66f, 0.9f, 0.96f, 1.0f};
        MTL::RenderCommandEncoder* m_Encoder;
        MTL::CommandBuffer* m_CommandBuffer;
        MetalPipeline* m_Pipeline;
    };
}
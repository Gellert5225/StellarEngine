#pragma once

#include "Stellar/Core/Core.h"

#include "Stellar/Renderer/RendererAPI.h"
#include "Stellar/Renderer/Texture.h"

#include "Stellar/Platform/Metal/MetalAPI.h"
#include "Stellar/Platform/Metal/Pipeline/MetalPipeline.h"
#include "Stellar/Platform/Metal/Buffer/MetalFrameBuffer.h"

namespace Stellar {
    class STLR_API MetalRenderer : public RendererAPI {
    public:
        void init() override;
        void shutDown() override;

        void beginRenderPass() override;
        void endRenderPass() override;

        void setClearColor(const glm::vec4& color) override;
        void renderGeometry(Buffer* vertexBuffer,
                            Buffer* indexBuffer,
                            Texture2D* texture,
                            const glm::vec4& color,
                            uint32_t indexCount = 0,
                            const glm::mat4& transform = {}) override;
        virtual void renderGrid(Buffer* vertexBuffer,
                                Buffer* indexBuffer,
                                uint32_t indexCount = 0) override;

        virtual void bindUbo(const GlobalUniforms& ubo) override;
        virtual FrameBuffer* getFrameBuffer() override;
        virtual void resizeFrameBuffer(uint32_t width, uint32_t height) override;

    private:
        MTL::ClearColor m_ClearColor = {0.66f, 0.9f, 0.96f, 1.0f};
        MTL::RenderCommandEncoder* m_Encoder;
        MTL::CommandBuffer* m_CommandBuffer;
        MetalPipeline* m_Pipeline;
        FrameBuffer* m_FrameBuffer;
        Buffer* m_UniformBuffer;
        NS::AutoreleasePool* pool;
    };
}
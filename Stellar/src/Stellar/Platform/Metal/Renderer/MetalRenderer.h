#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/STLRBase.h"

#include "Stellar/Renderer/UniformBuffer.h"
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

        void beginRenderPass(STLR_Ptr<CommandBuffer> commandBuffer, 
							STLR_Ptr<RenderPass> renderPass, 
							bool explicitClear = false) override;
        void endRenderPass(STLR_Ptr<CommandBuffer> commandBuffer) override;

		void beginFrame() override;

        void setClearColor(const glm::vec4& color) override;

		void renderGeometry(STLR_Ptr<CommandBuffer> renderCommandBuffer, 
							STLR_Ptr<Pipeline> pipeline,
							STLR_Ptr<UniformBufferSet> uniformBufferSet, 
							STLR_Ptr<Material> material, 
							STLR_Ptr<Buffer> vertexBuffer, 
							STLR_Ptr<Buffer> indexBuffer, 
							const glm::mat4& transform, 
							uint32_t indexCount = 0) override;

        virtual void renderGrid(STLR_Ptr<Buffer> vertexBuffer,
                                STLR_Ptr<Buffer> indexBuffer,
                                uint32_t indexCount = 0) override;

        virtual void bindUbo(const GlobalUniforms& ubo) override;
        virtual STLR_Ptr<FrameBuffer> getFrameBuffer() override;
        virtual void resizeFrameBuffer(uint32_t width, uint32_t height) override;

    private:
        MTL::ClearColor m_ClearColor = {0.66f, 0.9f, 0.96f, 1.0f};
        MTL::RenderCommandEncoder* m_Encoder;
        MTL::CommandBuffer* m_CommandBuffer;
		MTL::DepthStencilState* m_DepthStencilState;
        STLR_Ptr<MetalPipeline> m_Pipeline;
        STLR_Ptr<FrameBuffer> m_FrameBuffer;
        STLR_Ptr<UniformBuffer> m_UniformBuffer;

		dispatch_semaphore_t m_Semaphore;
    };
}

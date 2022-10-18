#pragma once

#include "Stellar/Core/Core.h"

#include "Stellar/Renderer/RendererAPI.h"
#include "Stellar/Renderer/Buffer.h"

#include "Stellar/Platform/Vulkan/SwapChain/VulkanSwapChain.h"
#include "Stellar/Platform/Vulkan/Buffer/VulkanFrameBuffer.h"
#include "Stellar/Core/Application.h"

#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API VulkanRenderer : public RendererAPI {
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
                            uint32_t indexCount,
                            const glm::mat4& transform) override;

        FrameBuffer* getFrameBuffer() override;
        void resizeFrameBuffer(uint32_t width, uint32_t height) override;
        
        void bindUbo(const GlobalUniforms& ubo) override;

        // vulkan
        static GraphicsPipeline* GetPipeline();
        static VkDescriptorSet AllocateDesriptorSet(VkDescriptorSetAllocateInfo& allocInfo);
    private:
        GraphicsPipeline* m_GraphicsPipeline = nullptr;
        VkClearColorValue m_ClearColor = {{0.66f, 0.9f, 0.96f, 1.0f}};

        Buffer* m_UniformBuffer{};
        FrameBuffer* m_FrameBuffer;

        VkDescriptorSet m_UboDescriptorSet;

        bool m_NeedResize = false;
        uint32_t m_ViewPortWidth, m_ViewPortHeight;

        void createUboDescriptorSet();
    };
}
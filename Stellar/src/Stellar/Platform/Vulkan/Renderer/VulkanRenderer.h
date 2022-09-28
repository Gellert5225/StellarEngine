#pragma once

#include "Stellar/Core.h"

#include "Stellar/Renderer/RendererAPI.h"
#include "Stellar/Renderer/Buffer.h"
#include "Stellar/Platform/Vulkan/SwapChain/VulkanSwapChain.h"
#include "Stellar/Application.h"

#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API VulkanRenderer : public RendererAPI {
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
                            const glm::vec3& color,
                            uint32_t indexCount,
                            const glm::mat4& transform) override;

        // vulkan
        static VkDescriptorSet AllocateDescriptorSets(VkDescriptorSetAllocateInfo& info);
        static std::vector<VkDescriptorSet>& GetDescriptorSets();
    private:
        GraphicsPipeline* m_GraphicsPipeline = nullptr;
        VkClearColorValue m_ClearColor = {{0.66f, 0.9f, 0.96f, 1.0f}};

        Buffer* m_UniformBuffer{};

        std::vector<VkDescriptorSet> m_DescriptorSets;

        void createDescriptorSets();
    };
}
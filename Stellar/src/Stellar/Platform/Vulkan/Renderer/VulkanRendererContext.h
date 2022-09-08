#pragma once

#include "Stellar/Core.h"
#include "Stellar/Renderer/RendererContext.h"
#include "Stellar/Platform/Vulkan/SwapChain/SwapChain.h"
#include "Stellar/Platform/Vulkan/Command/CommandBuffer.h"
#include "Stellar/Platform/Vulkan/Pipeline/GraphicsPipeline.h"
#include "Stellar/Platform/Vulkan/Instance/VulkanInstance.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

#include "Stellar/Window.h"

#include <memory>

namespace Stellar {
    class STLR_API VulkanRendererContext : public RendererContext {
    public:
        explicit VulkanRendererContext();
        ~VulkanRendererContext();

        [[nodiscard]] VkRenderPass getSwapChainRenderPass() const;
        [[nodiscard]] uint32_t getSwapChainImageCount() const;
        [[nodiscard]] int getCurrentFrameIndex() const;
        [[nodiscard]] VkExtent2D getSwapChainExtent() const;
        //[[nodiscard]] VkCommandBuffer getCurrentCommandBuffer() const;
        [[nodiscard]] VkFramebuffer getCurrentFrameBuffer() const;

        VkCommandBuffer beginFrame();
        void endFrame();

        void beginImGUIFrame();
        void endImGUIFrame();

        void beginRenderPass(VkCommandBuffer commandBuffer);
        void endRenderPass(VkCommandBuffer commandBuffer);
    private:
        uint32_t m_CurrentImageIndex = 0;
        int m_CurrentFrameIndex = 0;
        bool m_IsFrameStarted = false;

        std::unique_ptr<SwapChain> m_SwapChain;

        GraphicsPipeline* m_GraphicsPipeLine = nullptr;
        VulkanInstance* m_Instance = nullptr;
        VulkanDevice* m_Device = nullptr;

        void recreateSwapChain();
        void createPipeLine();

    };
}
#pragma once

#include "Stellar/Core.h"

#include "Stellar/Render/Vulkan/SwapChain/SwapChain.h"
#include "Stellar/Render/Vulkan/Command/CommandBuffer.h"
#include "Stellar/Render/Vulkan/Pipeline/GraphicsPipeline.h"
#include "Stellar/Render/Vulkan/Instance/VulkanInstance.h"
#include "Stellar/Render/Vulkan/Device/VulkanDevice.h"

#include "Stellar/Window.h"

#include <memory>

namespace Stellar {
    class STLR_API VulkanRendererContext {
    public:
        explicit VulkanRendererContext(Window &window);
        ~VulkanRendererContext();

        static VkCommandBuffer beginSingleTimeCommands();
        static void endSingleTimeCommands(VkCommandBuffer);

        [[nodiscard]] VkRenderPass getSwapChainRenderPass() const;
        [[nodiscard]] uint32_t getSwapChainImageCount() const;
        [[nodiscard]] int getCurrentFrameIndex() const;
        [[nodiscard]] VkExtent2D getSwapChainExtent() const;
        [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer() const;
        [[nodiscard]] VkFramebuffer getCurrentFrameBuffer() const;

        VkCommandBuffer beginFrame();
        void endFrame();

        void beginImGUIFrame();
        void endImGUIFrame();

        void beginRenderPass(VkCommandBuffer);
        void endRenderPass(VkCommandBuffer);
    private:
        uint32_t m_CurrentImageIndex = 0;
        int m_CurrentFrameIndex = 0;
        bool m_IsFrameStarted = false;

        Window& m_Window;

        std::unique_ptr<SwapChain> m_SwapChain;

        CommandBuffer* m_CommandBuffer = nullptr;
        GraphicsPipeline* m_GraphicsPipeLine = nullptr;
        VulkanInstance* m_Instance = nullptr;
        VulkanDevice* m_Device = nullptr;

        void recreateSwapChain();
        void createCommandBuffer();
        void createPipeLine();

    };
}
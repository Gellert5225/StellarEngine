#pragma once

#include "Stellar/Core.h"
#include "Stellar/Platform/Vulkan/Buffer/FrameBuffer.h"
#include "Stellar/Platform/Vulkan/Command/CommandBuffer.h"
#include "Stellar/Platform/Vulkan/RenderPass/StandardRenderPass.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

namespace Stellar {
    class STLR_API SwapChain {
    public:
        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

    public:
        SwapChain();
        explicit SwapChain(std::shared_ptr<SwapChain> oldSwapChain);
        ~SwapChain();

        void beginFrame();
        void present();

        [[nodiscard]] const std::vector<VkImage>* getSwapChainImages() const;
        [[nodiscard]] VkFormat getSwapChainImageFormat() const;
        [[nodiscard]] VkExtent2D getSwapChainExtent() const;
        [[nodiscard]] VkSwapchainKHR& getSwapChain();
        [[nodiscard]] VkRenderPass getRenderPass() const;
        [[nodiscard]] uint32_t getImageCount() const;
        [[nodiscard]] VkFramebuffer getFrameBuffer(uint32_t index) const;
        [[nodiscard]] bool compareSwapFormats(const SwapChain &swapChain) const;

        VkResult acquireNextImage(uint32_t* imageIndex);
        VkResult submitCommandBuffers(const VkCommandBuffer* buffer, const uint32_t* imageIndex);

        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    private:
        VkSwapchainKHR m_VulkanSwapChain = VK_NULL_HANDLE;

        VkFormat m_SwapChainImageFormat{};
        VkExtent2D m_SwapChainExtent{};

        FrameBuffer* m_FrameBuffer = nullptr;
        CommandBuffer* m_CommandBuffer = nullptr;
        StandardRenderPass* m_RenderPass = nullptr;

        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapChainImageViews;

        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;
        std::vector<VkFence> m_ImagesInFlight;

        std::shared_ptr<SwapChain> m_OldSwapChain;

        uint32_t m_CurrentFrameIndex = 0;
        uint32_t m_CurrentImageIndex = 0;

        void createSwapChain();
        void createImageViews();
        void createRenderPass();
        void createFrameBuffers();
        void createCommandBuffers();
        void createSemaphores();

        [[nodiscard]] static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
                const std::vector<VkSurfaceFormatKHR>&);
        [[nodiscard]] static VkPresentModeKHR chooseSwapPresentMode(
                const std::vector<VkPresentModeKHR>&);
        [[nodiscard]] static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&);
    };
}
#pragma once

#include "Stellar/Core.h"
#include "Stellar/Render/Vulkan/Buffer/FrameBuffer.h"
#include "Stellar/Render/Vulkan/RenderPass/StandardRenderPass.h"

#include <vulkan/vulkan.h>

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
        explicit SwapChain(std::shared_ptr<SwapChain>);
        ~SwapChain();

        [[nodiscard]] const std::vector<VkImage>* getSwapChainImages() const;
        [[nodiscard]] VkFormat getSwapChainImageFormat() const;
        [[nodiscard]] VkExtent2D getSwapChainExtent() const;
        [[nodiscard]] VkSwapchainKHR& getSwapChain();
        [[nodiscard]] VkRenderPass getRenderPass() const;
        [[nodiscard]] uint32_t getImageCount() const;
        [[nodiscard]] VkFramebuffer getFrameBuffer(uint32_t) const;
        [[nodiscard]] bool compareSwapFormats(const SwapChain &swapChain) const;

        VkResult acquireNextImage(uint32_t*);
        VkResult submitCommandBuffers(const VkCommandBuffer*, uint32_t*);

        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    private:
        VkSwapchainKHR m_VulkanSwapChain = VK_NULL_HANDLE;

        VkFormat m_SwapChainImageFormat{};
        VkExtent2D m_SwapChainExtent{};

        FrameBuffer* m_FrameBuffer = nullptr;
        StandardRenderPass* m_RenderPass = nullptr;

        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapChainImageViews;

        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;
        std::vector<VkFence> m_ImagesInFlight;

        std::shared_ptr<SwapChain> m_OldSwapChain;

        uint32_t m_CurrentFrame = 0;

        void createSwapChain();
        void createImageViews();
        void createRenderPass();
        void createFrameBuffers();
        void createSemaphores();

        [[nodiscard]] static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
                const std::vector<VkSurfaceFormatKHR>&);
        [[nodiscard]] static VkPresentModeKHR chooseSwapPresentMode(
                const std::vector<VkPresentModeKHR>&);
        [[nodiscard]] static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&);
    };
}
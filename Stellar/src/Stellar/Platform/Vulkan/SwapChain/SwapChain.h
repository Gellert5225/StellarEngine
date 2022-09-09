#pragma once

#include "Stellar/Core.h"
#include "Stellar/Platform/Vulkan/Buffer/FrameBuffer.h"
#include "Stellar/Platform/Vulkan/Command/VulkanCommandBuffer.h"
#include "Stellar/Platform/Vulkan/RenderPass/StandardRenderPass.h"
#include "Stellar/Platform/Vulkan/RenderPass/ImGuiRenderPass.h"

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
        ~SwapChain();

        void beginFrame();
        void present();
        void onResize();

        [[nodiscard]] const std::vector<VkImage>* getSwapChainImages() const;
        [[nodiscard]] VkFormat getSwapChainImageFormat() const;
        [[nodiscard]] VkExtent2D getSwapChainExtent() const;
        [[nodiscard]] VkSwapchainKHR& getSwapChain();
        [[nodiscard]] VkRenderPass getRenderPass() const;
        [[nodiscard]] VkRenderPass getImGuiRenderPass() const;
        [[nodiscard]] uint32_t getImageCount() const;
        [[nodiscard]] uint32_t getCurrentFrameIndex() const;
        [[nodiscard]] VkFramebuffer getCurrentFrameBuffer() const;
        [[nodiscard]] VkFramebuffer getCurrentImGuiFrameBuffer() const;
        [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer() const;
        [[nodiscard]] VkCommandBuffer getCommandBuffer(uint32_t index) const;

        static constexpr int MAX_FRAMES_IN_FLIGHT = 3;

    private:
        VkSwapchainKHR m_VulkanSwapChain = VK_NULL_HANDLE;

        VkFormat m_SwapChainImageFormat{};
        VkExtent2D m_SwapChainExtent{};

        StandardRenderPass* m_RenderPass = nullptr;
        ImGuiRenderPass* m_ImGuiRenderPass = nullptr;
        std::vector<VkFramebuffer> m_Framebuffers;
        std::vector<VkFramebuffer> m_ImGuiFramebuffers;

        struct SwapchainCommandBuffer {
            VkCommandPool CommandPool = nullptr;
            VkCommandBuffer CommandBuffer = nullptr;
        };
        std::vector<SwapchainCommandBuffer> m_CommandBuffers;

        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapChainImageViews;

        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;
        std::vector<VkFence> m_ImagesInFlight;

        uint32_t m_CurrentFrameIndex = 0;
        uint32_t m_CurrentImageIndex = 0;

        bool m_IsFrameStarted = false;

        void init();
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
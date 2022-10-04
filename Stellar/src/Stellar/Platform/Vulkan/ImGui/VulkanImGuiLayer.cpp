#include "stlrpch.h"
#include "VulkanImGuiLayer.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include "Stellar/ImGui/imgui_impl_glfw.h"
#include "Stellar/Platform/Vulkan/ImGui/imgui_impl_vulkan.h"

#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"
#include "Stellar/Platform/Vulkan/RenderPass/ImGuiRenderPass.h"

#include "Stellar/Core/Application.h"

namespace Stellar {
    static std::vector<VkCommandBuffer> s_ImGuiCommandBuffers;

    VulkanImGuiLayer::VulkanImGuiLayer() {
    }

    VulkanImGuiLayer::~VulkanImGuiLayer() {

    }

    void VulkanImGuiLayer::onAttach() {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        auto swapChain = (VulkanSwapChain*)Application::Get().getWindow().getSwapChain();
        VkDescriptorPoolSize pool_sizes[] = {
                {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
        };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;

        auto device = VulkanDevice::GetInstance()->logicalDevice();
        VK_CHECK_RESULT(vkCreateDescriptorPool(device, &pool_info, nullptr, &m_DescriptorPool));

        ImGui_ImplGlfw_InitForVulkan(Application::Get().getWindow().getGLFWWindow(), true);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = VulkanInstance::GetInstance()->getVkInstance();
        init_info.PhysicalDevice = VulkanDevice::GetInstance()->physicalDevice();
        init_info.Device = VulkanDevice::GetInstance()->logicalDevice();
        init_info.QueueFamily = VulkanDevice::GetInstance()->getIndices().graphicsFamily.value();
        init_info.Queue = VulkanDevice::GetInstance()->getGraphicsQueue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = m_DescriptorPool;
        init_info.Allocator = VK_NULL_HANDLE;
        init_info.MinImageCount = 2;
        init_info.ImageCount = swapChain->getImageCount();
        init_info.CheckVkResultFn = vulkanCheckResult;
        ImGui_ImplVulkan_Init(&init_info, swapChain->getRenderPass());

        VkCommandBuffer commandBuffer = VulkanDevice::GetInstance()->beginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
        VulkanDevice::GetInstance()->endSingleTimeCommands(commandBuffer);

        VK_CHECK_RESULT(vkDeviceWaitIdle(device));
        ImGui_ImplVulkan_DestroyFontUploadObjects();

        uint32_t framesInFlight = VulkanSwapChain::MAX_FRAMES_IN_FLIGHT;
        s_ImGuiCommandBuffers.resize(framesInFlight);
        for (uint32_t i = 0; i < framesInFlight; i++) {
            VkCommandBuffer cmdBuffer;

            VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
            cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmdBufAllocateInfo.commandPool = VulkanDevice::GetInstance()->getCommandPool();
            cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmdBufAllocateInfo.commandBufferCount = 1;

            VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, &cmdBuffer));
            s_ImGuiCommandBuffers[i] = cmdBuffer;
        }
    }

    void VulkanImGuiLayer::onDetach() {
        vkDestroyDescriptorPool(VulkanDevice::GetInstance()->logicalDevice(), m_DescriptorPool, nullptr);
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

//    void ImGuiLayer::onEvent(Event& e) {
//        ImGuiIO& io = ImGui::GetIO();
//        e.handled |= e.isInCategory(EventCategory::Mouse) & io.WantCaptureMouse;
//        e.handled |= e.isInCategory(EventCategory::Keyboard) & io.WantCaptureKeyboard;
//    }

    void VulkanImGuiLayer::begin() {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void VulkanImGuiLayer::end() const {
        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

        auto time = (float)glfwGetTime();
        io.DeltaTime = m_Time > 0.0 ? (time - m_Time) : (1.0f / 60.0f);

        ImGui::Render();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        auto swapChain = (VulkanSwapChain*)Application::Get().getWindow().getSwapChain();

#if 1
        VkClearValue clearValues[1];
        clearValues[0].color = { {0.1f, 0.1f,0.1f, 1.0f} };

//        auto commandBufferIndex = swapChain->getCurrentFrameIndex();

        VkCommandBufferBeginInfo drawCmdBufInfo = {};
        drawCmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        drawCmdBufInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK_RESULT(vkBeginCommandBuffer(swapChain->getCurrentCommandBuffer(), &drawCmdBufInfo));

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.pNext = nullptr;
        renderPassBeginInfo.renderPass = swapChain->getImGuiRenderPass();
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent.width = swapChain->getSwapChainExtent().width;
        renderPassBeginInfo.renderArea.extent.height = swapChain->getSwapChainExtent().height;
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = clearValues;
        renderPassBeginInfo.framebuffer = swapChain->getCurrentFrameBuffer();

        vkCmdBeginRenderPass(swapChain->getCurrentCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport = {};
        viewport.x = 0;
        viewport.y = 1;
        viewport.height = io.DisplaySize.x;
        viewport.width = io.DisplaySize.y;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(swapChain->getCurrentCommandBuffer(), 0, 1, &viewport);

        // VkRect2D scissor = {};
        // scissor.extent.width = swapChain->getSwapChainExtent().width;
        // scissor.extent.height = swapChain->getSwapChainExtent().height;
        // scissor.offset.x = 0;
        // scissor.offset.y = 0;
        // vkCmdSetScissor(swapChain->getCurrentCommandBuffer(), 0, 1, &scissor);

        ImDrawData* main_draw_data = ImGui::GetDrawData();
        
        ImGui_ImplVulkan_RenderDrawData(main_draw_data, swapChain->getCurrentCommandBuffer());

        vkCmdEndRenderPass(swapChain->getCurrentCommandBuffer());

        VK_CHECK_RESULT(vkEndCommandBuffer(swapChain->getCurrentCommandBuffer()));
#else
        ImDrawData *drawdata = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(drawdata, (VkCommandBuffer)commandBuffer->getActiveCommandBuffer());
#endif
    }

    void VulkanImGuiLayer::onImGuiRender() {
  
    }
}
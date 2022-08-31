#include "stlrpch.h"
#include "ImGuiLayer.h" 

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "Stellar/Render/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Render/Vulkan/VulkanCommon.h"

#include "Stellar/Application.h"

namespace Stellar {
    static std::vector<VkCommandBuffer> s_ImGuiCommandBuffers;

    ImGuiLayer::ImGuiLayer() 
        : Layer("ImGuiLayer") {
        
    }

    ImGuiLayer::~ImGuiLayer() {

    }

    void ImGuiLayer::onAttach() {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

        VkDescriptorPool descriptorPool;
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

        if (vkCreateDescriptorPool(VulkanDevice::GetInstance()->logicalDevice(),
            &pool_info, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up descriptor pool for imgui");
        }

        ImGui_ImplGlfw_InitForVulkan(Application::Get().getWindow().getGLFWWindow(), true);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = VulkanInstance::GetInstance()->getVkInstance();
        init_info.PhysicalDevice = VulkanDevice::GetInstance()->physicalDevice();
        init_info.Device = VulkanDevice::GetInstance()->logicalDevice();
        init_info.QueueFamily = VulkanDevice::GetInstance()->getIndices().graphicsFamily.value();
        init_info.Queue = VulkanDevice::GetInstance()->getGraphicsQueue();

        // pipeline cache is a potential future optimization, ignoring for now
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = descriptorPool;
        init_info.Allocator = VK_NULL_HANDLE;
        init_info.MinImageCount = 2;
        init_info.ImageCount = Application::Get().getRendererContext()->getSwapChainImageCount();
        init_info.CheckVkResultFn = vulkanCheckResult;
        ImGui_ImplVulkan_Init(&init_info,
                              Application::Get().getRendererContext()->getSwapChainRenderPass());

        VkCommandBuffer commandBuffer = VulkanRendererContext::beginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
        VulkanRendererContext::endSingleTimeCommands(commandBuffer);

        //VK_CHECK_RESULT(vkDeviceWaitIdle(VulkanDevice::GetInstance()->logicalDevice()));
        ImGui_ImplVulkan_DestroyFontUploadObjects();
//
//        uint32_t framesInFlight = SwapChain::MAX_FRAMES_IN_FLIGHT;
//        auto* second = new CommandBuffer(VulkanDevice::GetInstance()->getCommandPool(),
//                                                  framesInFlight, VK_COMMAND_BUFFER_LEVEL_SECONDARY);
//        s_ImGuiCommandBuffers = *second->getCommandBuffers();
    }

    void ImGuiLayer::onDetach() {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::onUpdate() {

    }

    void ImGuiLayer::onEvent(Event& e) {

    }

    void ImGuiLayer::begin() const {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::end(VkCommandBuffer commandBuffer) const {
        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

        auto time = (float)glfwGetTime();
        io.DeltaTime = m_Time > 0.0 ? (time - m_Time) : (1.0f / 60.0f);

        ImGui::Render();
//
//        auto context = Application::Get().getRendererContext();
//
//        VkClearValue clearValues[2];
//        clearValues[0].color = { {0.1f, 0.1f,0.1f, 1.0f} };
//        clearValues[1].depthStencil = { 1.0f, 0 };
//
//        int commandBufferIndex = context->getCurrentFrameIndex();
//
//        VkCommandBufferBeginInfo drawCmdBufInfo = {};
//        drawCmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//        drawCmdBufInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//        drawCmdBufInfo.pNext = nullptr;
//
//        VkCommandBuffer drawCommandBuffer = context->getCurrentCommandBuffer();
//        VK_CHECK_RESULT(vkBeginCommandBuffer(drawCommandBuffer, &drawCmdBufInfo));
//
//        VkRenderPassBeginInfo renderPassBeginInfo = {};
//        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//        renderPassBeginInfo.pNext = nullptr;
//        renderPassBeginInfo.renderPass = context->getSwapChainRenderPass();
//        renderPassBeginInfo.renderArea.offset.x = 0;
//        renderPassBeginInfo.renderArea.offset.y = 0;
//        renderPassBeginInfo.renderArea.extent = context->getSwapChainExtent();
//        renderPassBeginInfo.clearValueCount = 2; // Color + depth
//        renderPassBeginInfo.pClearValues = clearValues;
//        renderPassBeginInfo.framebuffer = context->getCurrentFrameBuffer();
//
//        vkCmdBeginRenderPass(drawCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
//
//        VkCommandBufferInheritanceInfo inheritanceInfo = {};
//        inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
//        inheritanceInfo.renderPass = context->getSwapChainRenderPass();
//        inheritanceInfo.framebuffer = context->getCurrentFrameBuffer();
//
//        VkCommandBufferBeginInfo cmdBufInfo = {};
//        cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//        cmdBufInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
//        cmdBufInfo.pInheritanceInfo = &inheritanceInfo;
//
//        VK_CHECK_RESULT(vkBeginCommandBuffer(s_ImGuiCommandBuffers[commandBufferIndex], &cmdBufInfo));
//
//        VkViewport viewport = {};
//        viewport.x = 0.0f;
//        viewport.y = (float)context->getSwapChainExtent().height;
//        viewport.height = (float)context->getSwapChainExtent().height;
//        viewport.width = (float)context->getSwapChainExtent().width;
//        viewport.minDepth = 0.0f;
//        viewport.maxDepth = 1.0f;
//        vkCmdSetViewport(s_ImGuiCommandBuffers[commandBufferIndex], 0, 1, &viewport);
//
//        VkRect2D scissor = {};
//        scissor.extent.width = context->getSwapChainExtent().width;
//        scissor.extent.height = context->getSwapChainExtent().height;
//        scissor.offset.x = 0;
//        scissor.offset.y = 0;
//        vkCmdSetScissor(s_ImGuiCommandBuffers[commandBufferIndex], 0, 1, &scissor);
//
//        ImDrawData* main_draw_data = ImGui::GetDrawData();
//        ImGui_ImplVulkan_RenderDrawData(main_draw_data, s_ImGuiCommandBuffers[commandBufferIndex]);
//
//        VK_CHECK_RESULT(vkEndCommandBuffer(s_ImGuiCommandBuffers[commandBufferIndex]));
//
//        std::vector<VkCommandBuffer> commandBuffers;
//        commandBuffers.push_back(s_ImGuiCommandBuffers[commandBufferIndex]);
//
//        vkCmdExecuteCommands(drawCommandBuffer, uint32_t(commandBuffers.size()), commandBuffers.data());
//
//        vkCmdEndRenderPass(drawCommandBuffer);
//
//        VK_CHECK_RESULT(vkEndCommandBuffer(drawCommandBuffer));
        ImDrawData *drawdata = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(drawdata, commandBuffer);
    }

    void ImGuiLayer::onImGuiRender() {
        static bool show = true;
        ImGui::ShowDemoWindow(&show);
    }
}
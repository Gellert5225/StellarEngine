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

		setDarkThemeColor();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		io.Fonts->AddFontFromFileTTF("Resources/Fonts/OpenSans/static/OpenSans/OpenSans-Bold.ttf", 18.0f);

		io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/OpenSans/OpenSans-VariableFont_wdth,wght.ttf", 18.0f);

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
		ImGui_ImplVulkan_Init(&init_info, swapChain->getImGuiRenderPass());

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
			cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
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

	void VulkanImGuiLayer::end() {
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Render();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		auto swapChain = (VulkanSwapChain*)Application::Get().getWindow().getSwapChain();

		VkClearValue clearValues[2];
		clearValues[0].color = { {0.1f, 0.1f,0.1f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkCommandBufferBeginInfo drawCmdBufInfo = {};
		drawCmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		drawCmdBufInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		drawCmdBufInfo.pNext = nullptr;

		uint32_t commandBufferIndex = swapChain->getCurrentFrameIndex();

		VK_CHECK_RESULT(vkBeginCommandBuffer(swapChain->getCurrentCommandBuffer(), &drawCmdBufInfo));

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = swapChain->getImGuiRenderPass();
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = swapChain->getSwapChainExtent().width;
		renderPassBeginInfo.renderArea.extent.height = swapChain->getSwapChainExtent().height;
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;
		renderPassBeginInfo.framebuffer = swapChain->getCurrentFrameBuffer();

		vkCmdBeginRenderPass(swapChain->getCurrentCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		VkCommandBufferInheritanceInfo inheritanceInfo = {};
		inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		inheritanceInfo.renderPass = swapChain->getImGuiRenderPass();
		inheritanceInfo.framebuffer = swapChain->getCurrentFrameBuffer();

		VkCommandBufferBeginInfo cmdBufInfo = {};
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		cmdBufInfo.pInheritanceInfo = &inheritanceInfo;

		VK_CHECK_RESULT(vkBeginCommandBuffer(s_ImGuiCommandBuffers[commandBufferIndex], &cmdBufInfo));

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = (float)swapChain->getSwapChainExtent().height;
		viewport.height = -(float)swapChain->getSwapChainExtent().height;
		viewport.width = (float)swapChain->getSwapChainExtent().width;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(s_ImGuiCommandBuffers[commandBufferIndex], 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.extent.width = swapChain->getSwapChainExtent().width;
		scissor.extent.height = swapChain->getSwapChainExtent().height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(s_ImGuiCommandBuffers[commandBufferIndex], 0, 1, &scissor);

		ImDrawData* main_draw_data = ImGui::GetDrawData();
		
		vkDeviceWaitIdle(VulkanDevice::GetInstance()->logicalDevice());

		ImGui_ImplVulkan_RenderDrawData(main_draw_data, s_ImGuiCommandBuffers[commandBufferIndex]);

		VK_CHECK_RESULT(vkEndCommandBuffer(s_ImGuiCommandBuffers[commandBufferIndex]));

		std::vector<VkCommandBuffer> commandBuffers;
		commandBuffers.push_back(s_ImGuiCommandBuffers[commandBufferIndex]);

		vkCmdExecuteCommands(swapChain->getCurrentCommandBuffer(), uint32_t(commandBuffers.size()), commandBuffers.data());

		vkCmdEndRenderPass(swapChain->getCurrentCommandBuffer());

		VK_CHECK_RESULT(vkEndCommandBuffer(swapChain->getCurrentCommandBuffer()));
	}

	void VulkanImGuiLayer::onImGuiRender() {

	}

	void VulkanImGuiLayer::setDarkThemeColor() {
		auto& colors = ImGui::GetStyle().Colors;

		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		
		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}
}
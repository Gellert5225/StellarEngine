#pragma once

#include "Stellar/Core/Layer.h"
#include "Stellar/Events/KeyEvent.h"
#include "Stellar/Events/MouseEvent.h"
#include "Stellar/Events/ApplicationEvent.h"

#include "Stellar/Renderer/CommandBuffer.h"

#include "Stellar/ImGui/ImGuiLayer.h"
#include <vulkan/vulkan.h>

namespace Stellar {
	class STLR_API VulkanImGuiLayer: public ImGuiLayer {
	public:
		VulkanImGuiLayer();
		~VulkanImGuiLayer() override;

		void begin() override;
		void end() override;

		void onAttach() override;
		void onDetach() override;
		void onImGuiRender() override;

		void onEvent(Event& e) override;
	//static std::vector<VkCommandBuffer> s_ImGuiCommandBuffers;
	private:
		VkDescriptorPool m_DescriptorPool;
	};
}
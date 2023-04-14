#pragma once

#include "Stellar/Core/Core.h"

#include "VulkanPipeline.h"
#include "Stellar/Platform/Vulkan/Shader/VulkanShader.h"

namespace Stellar {
	class STLR_API GraphicsPipeline : public VulkanPipeline {
	public:
		GraphicsPipeline(Shader* shader, VkRenderPass renderPass);
	protected:
		void createDescriptorSetLayout() override;
		void createDescriptorPool() override;
	};
}
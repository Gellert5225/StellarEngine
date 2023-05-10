#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Platform/Vulkan/Queue/Queue.h"
#include <vulkan/vulkan.h>

namespace Stellar {
	class CommandPool {
	public:
		static CommandPool* GetInstance();

		~CommandPool();

		void init(Queue::QueueFamilyIndices indices);
		void reset();
		VkCommandPool* getVkCommandPool();
	private:
		static CommandPool* s_Instance;
		VkCommandPool commandPool;

		CommandPool() = default;
	};
}
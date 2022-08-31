#pragma once

#include "Stellar/Core.h"
#include "Stellar/Render/Vulkan/Queue/Queue.h"
#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API CommandPool {
    public:
        static CommandPool* GetInstance();

        ~CommandPool();

        void init(Queue::QueueFamilyIndices);
        void reset();
        VkCommandPool* getVkCommandPool();
    private:
        static CommandPool* s_Instance;
        VkCommandPool commandPool;

        CommandPool() = default;
    };
}
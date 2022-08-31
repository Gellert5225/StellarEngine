#pragma once

#include "Stellar/Core.h"

#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API Shader {
    public:
        static Shader* GetInstance();

        ~Shader();

        void cleanUp();
        VkShaderModule getShaderModule(const std::string&);
    private:
        static Shader* s_Instance;
        static VkShaderModule CreateShaderModule(const std::vector<char>&);
        static std::vector<char> ReadFile(const std::string&);

        std::unordered_map<std::string, VkShaderModule> loadedShaders;

        Shader() = default;
    };
}
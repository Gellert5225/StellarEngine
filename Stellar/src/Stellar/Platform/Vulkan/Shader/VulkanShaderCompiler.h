#pragma once

#include "Stellar/Core.h"

#include "Stellar/Renderer/Shader.h"

#include <unordered_map>
#include <vector>
#include <shaderc/shaderc.hpp>

namespace Stellar {
    namespace VulkanShaderCompiler {
        /* Take the source code of the shader, extract vertex and fragment shader, 
           then return the mapped shaders */
        static std::unordered_map<ShaderType, std::string> PreProcess(const std::string& source);

        static void Compile(std::unordered_map<ShaderType, std::vector<uint32_t>>& spvOutput);
    }

    namespace VulkanShaderCompilerUtil {

    }
}
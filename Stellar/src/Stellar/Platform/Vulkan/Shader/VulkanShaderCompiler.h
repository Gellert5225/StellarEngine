#pragma once

#include "Stellar/Core.h"

#include "Stellar/Renderer/Shader.h"

#include <unordered_map>
#include <vector>
#include <shaderc/shaderc.hpp>

#include <cstring>

#include "Stellar/Log.h"
#include <sstream>
#include <iostream>

namespace Stellar {
    namespace VulkanShaderCompilerUtil {
        static ShaderType ShaderTypeFromString(const std::string& type) {
            if (type == "vert") return ShaderType::Vertex;
            if (type == "frag") return ShaderType::Fragment;
            return ShaderType::None;
        }
    }

    namespace VulkanShaderCompiler {
        /* Take the source code of the shader, extract vertex and fragment shader, 
           then return the mapped shaders */
        static std::unordered_map<ShaderType, std::string> PreProcess(const std::string& source) {
            std::unordered_map<ShaderType, std::string> shaderSources;

            std::string shaderSource = "";
            bool recording = false;

            ShaderType type = ShaderType::None;
            std::istringstream s(source);
            std::string line;
            while (std::getline(s, line)) {
                std::istringstream iss(line);
                std::string word;
                std::string prevWord;
                while(iss >> word) {
                    if (!recording && word == "#version")
                        recording = true;
                    else if (recording && word == "#version") {
                        shaderSources[type] = shaderSource;
                        shaderSource = "";
                        shaderSource += line;
                        type = ShaderType::None;
                        break;
                    }

                    if (prevWord == "#pragma" && (word == "vert" || word == "frag")) {
                        type = VulkanShaderCompilerUtil::ShaderTypeFromString(word);
                    }
                    
                    prevWord = word;
                }

                shaderSource += line + "\n";
            }

            shaderSources[type] = shaderSource;

            return shaderSources;
        }

        static void Compile(std::unordered_map<ShaderType, std::vector<uint32_t>>& spvOutput);
    }
}
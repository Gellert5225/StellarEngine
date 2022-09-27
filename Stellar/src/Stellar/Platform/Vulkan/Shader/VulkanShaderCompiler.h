#pragma once

#include <sstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <shaderc/shaderc.hpp>

#include "Stellar/Core.h"
#include "Stellar/Log.h"
#include "Stellar/Renderer/Shader.h"

namespace Stellar {
    namespace VulkanShaderCompilerUtil {
        static ShaderType ShaderTypeFromString(const std::string& type) {
            if (type == "vert") return ShaderType::Vertex;
            if (type == "frag") return ShaderType::Fragment;
            return ShaderType::None;
        }

        static shaderc_shader_kind ShadercTypeFromType(const ShaderType type) {
            if (type == ShaderType::Vertex) return shaderc_vertex_shader;
            if (type == ShaderType::Fragment) return shaderc_fragment_shader;
            return shaderc_miss_shader;
        }

        static VkShaderStageFlagBits VulkanShaderTypeFromType(const ShaderType type) {
            if (type == ShaderType::Vertex) return VK_SHADER_STAGE_VERTEX_BIT;
            if (type == ShaderType::Fragment) return VK_SHADER_STAGE_FRAGMENT_BIT;
            return VK_SHADER_STAGE_ALL;
        }

        static std::string ShadercCompileErrorToString(const shaderc_compilation_status status) {
            if (status == shaderc_compilation_status_invalid_stage) return "shaderc_compilation_status_invalid_stage";
            if (status == shaderc_compilation_status_compilation_error) return "shaderc_compilation_status_compilation_error";
            if (status == shaderc_compilation_status_internal_error) return "shaderc_compilation_status_internal_error";
            if (status == shaderc_compilation_status_null_result_object) return "shaderc_compilation_status_null_result_object";
            if (status == shaderc_compilation_status_invalid_assembly) return "shaderc_compilation_status_invalid_assembly";
            if (status == shaderc_compilation_status_validation_error) return "shaderc_compilation_status_validation_error"; 
            if (status == shaderc_compilation_status_transformation_error) return "shaderc_compilation_status_transformation_error"; 
            if (status == shaderc_compilation_status_configuration_error) return "shaderc_compilation_status_configuration_error"; 
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

        static void Compile(const std::unordered_map<ShaderType, std::string>& shaderSources, 
                            std::unordered_map<ShaderType, std::vector<uint32_t>>& spvOutput) {
            shaderc::Compiler compiler;
            shaderc::CompileOptions options;
            options.SetOptimizationLevel(shaderc_optimization_level_size);

            for (auto& shader : shaderSources) {
                shaderc::SpvCompilationResult module =
                    compiler.CompileGlslToSpv(shader.second, 
                                              VulkanShaderCompilerUtil::ShadercTypeFromType(shader.first),
                                              "shader", options);

                if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
                    STLR_CORE_ASSERT(false, "Failed to compile shader " + module.GetErrorMessage());
                }
                spvOutput[shader.first] = {module.cbegin(), module.cend()};
            }
        }
    }
}
#pragma once

#include <sstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <shaderc/shaderc.hpp>
#include <vulkan/vulkan.h>

#include "Stellar/Core/Core.h"
#include "Stellar/Core/Log.h"
#include "Stellar/Renderer/Shader.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>

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

		static ShaderUniformType SPIRTypeToShaderUniformType(spirv_cross::SPIRType type) {
			switch (type.basetype)
			{
			case spirv_cross::SPIRType::Boolean:  return ShaderUniformType::Bool;
			case spirv_cross::SPIRType::Int:      return ShaderUniformType::Int;
			case spirv_cross::SPIRType::UInt:     return ShaderUniformType::UInt;
			case spirv_cross::SPIRType::Float:
				if (type.vecsize == 1)            return ShaderUniformType::Float;
				if (type.vecsize == 2)            return ShaderUniformType::Vec2;
				if (type.vecsize == 3)            return ShaderUniformType::Vec3;
				if (type.vecsize == 4)            return ShaderUniformType::Vec4;

				if (type.columns == 3)            return ShaderUniformType::Mat3;
				if (type.columns == 4)            return ShaderUniformType::Mat4;
				break;
			}
			STLR_CORE_ASSERT(false, "Unknown type!");
			return ShaderUniformType::None;
		}
	}

	namespace VulkanShaderCompiler {
		/* Take the source code of the shader, extract vertex and fragment shader, 
		then return the mapped shaders */
		std::unordered_map<ShaderType, std::string> PreProcess(const std::string& source);

		void Compile(const std::unordered_map<ShaderType, std::string>& shaderSources, 
							std::unordered_map<ShaderType, std::vector<uint32_t>>& spvOutput);
	}
}
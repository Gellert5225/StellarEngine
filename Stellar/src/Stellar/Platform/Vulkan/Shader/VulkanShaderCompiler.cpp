#include "stlrpch.h"
#include "VulkanShaderCompiler.h"

namespace Stellar {
	namespace VulkanShaderCompiler {
		std::unordered_map<ShaderType, std::string> STLR_API PreProcess(const std::string& source) {
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

					if (prevWord == "#pragma" && (word == "vert" || word == "frag"))
						type = VulkanShaderCompilerUtil::ShaderTypeFromString(word);
					
					prevWord = word;
				}

				shaderSource += line + "\n";
			}

			shaderSources[type] = shaderSource;

			return shaderSources;
		}

		void STLR_API Compile(const std::unordered_map<ShaderType, std::string>& shaderSources, 
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
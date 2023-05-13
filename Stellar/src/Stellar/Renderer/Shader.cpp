#include "stlrpch.h"
#include "Shader.h"

#include "Stellar/Renderer/RendererAPI.h"

#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Shader/VulkanShader.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Shader/MetalShader.h"
#endif

#include "Stellar/Core/Log.h"

namespace Stellar {
	STLR_Ptr<Shader> Shader::Create(const std::string& filePath) {
		switch (RendererAPI::Current()) {
			case RendererAPIType::Vulkan:
			#if defined(__linux__) || defined(_WIN64)
				return STLR_Ptr<VulkanShader>::Create(filePath);
			#endif
			case RendererAPIType::Metal:
			#if defined(__APPLE__)
				return new MetalShader(filePath);
			#endif
			case RendererAPIType::None:
				break;
		}
		STLR_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	const std::string Shader::ReadFile(const std::string& fileName) {
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);
		if (!file.is_open())
			throw std::runtime_error("Failed to open shader file " + fileName);

		size_t fileSize = (size_t) file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return std::string(buffer.begin(), buffer.end());
	}

	const std::string Shader::extractName(const std::string& filePath) const {
		// xx/xx/shader.glsl
		auto lastDotPos = filePath.find_last_of(".");
		auto lastSlashPos = filePath.find_last_of("/");
		return filePath.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1);
	}

	ShaderLibrary::~ShaderLibrary() {
		m_Shaders.clear();
	}

	void ShaderLibrary::add(const std::string& name, STLR_Ptr<Shader> shader) {
		STLR_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(), "Shader " + name + " already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::add(STLR_Ptr<Shader> shader) {
		auto& name = shader->getName();
		add(name, shader);
	}

	void ShaderLibrary::load(const std::string& fileName) {
		// get filepath based on platform
		std::string filePath;
		#if defined(_WIN64) || defined(__linux__)
		filePath = "Resources/Shader/Vulkan/" + fileName + ".glsl";
		#elif defined(__APPLE__)
		filePath = "./Debug/Resources/Shader/Metal/" + fileName + ".metal";
		#endif
		auto shader = Shader::Create(filePath);
		add(shader);
	}

	void ShaderLibrary::load(const std::string& name, const std::string& fileName) {
		// get filepath based on platform
		std::string filePath;
		#if defined(_WIN64) || defined(__linux__)
		filePath = "../../Resources/Shader/Vulkan/" + fileName + ".glsl";
		#elif defined(__APPLE__)
		filePath = "./Debug/Resources/Shader/Metal/" + fileName + ".metal";
		#endif
		auto shader = Shader::Create(filePath);
		add(name, shader);
	}

	STLR_Ptr<Shader> ShaderLibrary::get(const std::string& name) {
		STLR_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end(), "Shader " + name + " not found");
		return m_Shaders[name];
	}

	ShaderUniform::ShaderUniform(const std::string& name, ShaderUniformType type, uint32_t size, uint32_t offset)
		: m_Name(name), m_Type(type), m_Size(size), m_Offset(offset)
	{
	}

	const std::string ShaderUniform::UniformTypeToString(ShaderUniformType type)
	{
		if (type == ShaderUniformType::Bool)
			return "Boolean";
		else if (type == ShaderUniformType::Int)
			return "Int";
		else if (type == ShaderUniformType::Float)
			return "Float";

		return "None";
	}
}

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
    Shader* Shader::Create(const std::string& filePath) {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan:
            #if defined(__linux__) || defined(_WIN64)
                return new VulkanShader(filePath);
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

    Shader::Shader(const std::string& filePath) {
        m_Name = extractName(filePath);
    }

    const std::string Shader::extractName(const std::string& filePath) const {
        // xx/xx/shader.glsl
        auto lastDotPos = filePath.find_last_of(".");
        auto lastSlashPos = filePath.find_last_of("/");
        return filePath.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1);
    }

    ShaderLibrary::~ShaderLibrary() {
        for (auto kv : m_Shaders) {
            delete kv.second;
        }
        m_Shaders.clear();
    }

    void ShaderLibrary::add(const std::string& name, Shader* shader) {
        STLR_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(), "Shader " + name + " already exists!");
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::add(Shader* shader) {
        auto& name = shader->getName();
        add(name, shader);
    }

    void ShaderLibrary::load(const std::string& filePath) {
        auto shader = Shader::Create(filePath);
        add(shader);
    }

    void ShaderLibrary::load(const std::string& name, const std::string& filePath) {
        auto shader = Shader::Create(filePath);
        add(name, shader);
    }

    Shader* ShaderLibrary::get(const std::string& name) {
        STLR_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end(), "Shader " + name + " not found");
        return m_Shaders[name];
    }
}

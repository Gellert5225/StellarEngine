#include "stlrpch.h"
#include "MetalShader.h"

#include "Stellar/Core/Log.h"

#include "Stellar/Platform/Metal/Device/MetalDevice.h"

namespace Stellar {
    MetalShader::MetalShader(const std::string& filePath) {
        auto shaderSrc = Shader::ReadFile(filePath);

        NS::Error* error = nullptr;
        m_Library = MetalDevice::GetInstance()->getDevice()->newLibrary(
            NS::String::string(shaderSrc.data(), NS::StringEncoding::UTF8StringEncoding), 
            nullptr, 
            &error);
        STLR_CORE_ASSERT(m_Library,  error->localizedDescription()->utf8String());
        STLR_CORE_INFO("Loaded shader {0}", filePath);
    }

    MTL::Library* MetalShader::getLibrary() {
        return m_Library;
    }

    MetalShader::~MetalShader() {
        m_Library->release();
    }
}

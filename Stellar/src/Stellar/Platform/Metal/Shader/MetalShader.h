#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Shader.h"

#include "Stellar/Platform/Metal/MetalAPI.h"

namespace Stellar {
    class STLR_API MetalShader : public Shader {
    public:
        MetalShader(const std::string& filePath);
        ~MetalShader();

        MTL::Library* getLibrary();
    private:
        MTL::Library* m_Library;
    };
}
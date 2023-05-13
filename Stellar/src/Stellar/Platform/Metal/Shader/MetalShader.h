#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Shader.h"

#include "Stellar/Platform/Metal/MetalAPI.h"

namespace Stellar {
    class MetalShader : public Shader {
    public:
        MetalShader(const std::string& filePath);
        ~MetalShader();

		virtual const std::unordered_map<std::string, ShaderResourceDeclaration>& getResources() const override;

        MTL::Library* getLibrary();
	private:
		void reflect();
		void reflectArguments(const std::unordered_map<std::string, MTL::Argument*>& vertexArgs);
		void processShaderStruct(NS::Array* members);
		static std::string MetalShaderDatatypeToString(MTL::DataType type);
		static std::string MetalShaderTextureTypeToString(MTL::TextureType type);
	private:
        MTL::Library* m_Library;
		std::unordered_map<std::string, ShaderResourceDeclaration> m_Resources;
    };
}
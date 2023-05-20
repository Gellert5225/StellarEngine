#include "stlrpch.h"
#include "MetalShader.h"

#include "Stellar/Core/Log.h"

#include "Stellar/Platform/Metal/Device/MetalDevice.h"

namespace Stellar {
    MetalShader::MetalShader(const std::string& filePath): Shader(filePath) {
        auto shaderSrc = Shader::ReadFile(filePath);

        NS::Error* error = nullptr;
        m_Library = MetalDevice::GetInstance()->getDevice()->newLibrary(
            NS::String::string(shaderSrc.data(), NS::StringEncoding::UTF8StringEncoding), 
            nullptr, 
            &error);
        STLR_CORE_ASSERT(m_Library,  error->localizedDescription()->utf8String());
        STLR_CORE_INFO("Loaded shader {0}", filePath);

		//reflect();
    }

    MTL::Library* MetalShader::getLibrary() {
        return m_Library;
    }

    MetalShader::~MetalShader() {
        m_Library->release();
    }

	void MetalShader::reflect() {
		MTL::Function* vertexFn = m_Library->newFunction(NS::String::string("vertexMain", NS::StringEncoding::UTF8StringEncoding));
        MTL::Function* fragFn = m_Library->newFunction(NS::String::string("fragmentMain", NS::StringEncoding::UTF8StringEncoding));

		MTL::RenderPipelineDescriptor* descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
        descriptor->setVertexFunction(vertexFn);
        descriptor->setFragmentFunction(fragFn);

		NS::Error* error = nullptr; 
		MTL::RenderPipelineReflection* reflectionObj;
		MTL::PipelineOption option = MTL::PipelineOptionBufferTypeInfo | MTL::PipelineOptionArgumentInfo;
		auto pipelineState = MetalDevice::GetInstance()->getDevice()->newRenderPipelineState(descriptor, option, &reflectionObj, &error);

		std::unordered_map<std::string, MTL::Argument*> vertexArgs;
		std::unordered_map<std::string, MTL::Argument*> fragmentArgs;

		for (int i = 0; i < reflectionObj->vertexArguments()->count(); i++) {
			MTL::Argument *arg = (MTL::Argument*)reflectionObj->vertexArguments()->object(i);
			vertexArgs[arg->name()->utf8String()] = arg;
		}

		for (int i = 0; i < reflectionObj->fragmentArguments()->count(); i++) {
			MTL::Argument *arg = (MTL::Argument*)reflectionObj->fragmentArguments()->object(i);
			fragmentArgs[arg->name()->utf8String()] = arg;
		}

		STLR_CORE_TRACE("===========================");
		STLR_CORE_TRACE("Metal Shader Reflection");
		STLR_CORE_TRACE("Shader: {0}", m_FilePath);
		STLR_CORE_TRACE("Stage: Vertex");
		STLR_CORE_TRACE("===========================");

		reflectArguments(vertexArgs);

		STLR_CORE_TRACE("===========================");
		STLR_CORE_TRACE("Metal Shader Reflection");
		STLR_CORE_TRACE("Shader: {0}", m_FilePath);
		STLR_CORE_TRACE("Stage: Fragment");
		STLR_CORE_TRACE("===========================");
		reflectArguments(fragmentArgs);

		pipelineState->release();
		descriptor->release();
        vertexFn->release();
        fragFn->release();
	}

	void MetalShader::reflectArguments(const std::unordered_map<std::string, MTL::Argument*>& vertexArgs) {
		for (auto [name, arg] : vertexArgs) {
			auto type = arg->type();

			if (type == MTL::ArgumentTypeBuffer) {
				auto bufferDatatype = arg->bufferDataType();
				STLR_CORE_INFO("  ===================================================");
				STLR_CORE_INFO("  | Argument: (Name: {0}, Type: {1}, Index: {2}, Size: {3}, Alignment: {4})", name, MetalShader::MetalShaderDatatypeToString(bufferDatatype), arg->index(), arg->bufferDataSize(), arg->bufferAlignment());
				STLR_CORE_INFO("  ===================================================");
				if (bufferDatatype == MTL::DataType::DataTypeStruct) {
					auto members = arg->bufferStructType()->members();
					processShaderStruct(members);
				}
			} else if (type == MTL::ArgumentTypeSampler) {
				STLR_CORE_INFO("Sampler");
			} else if (type == MTL::ArgumentTypeTexture) {
				STLR_CORE_INFO("  ===================================================");
				STLR_CORE_INFO("  | Argument: (Name: {0}, Type: {1}, TextureType: {2}, Index: {3})", name, MetalShader::MetalShaderTextureTypeToString(arg->textureType()), MetalShader::MetalShaderDatatypeToString(arg->textureDataType()), arg->index());
				STLR_CORE_INFO("  ===================================================");
			}
		}
	}

	const std::unordered_map<std::string, ShaderResourceDeclaration>& MetalShader::getResources() const {
		return m_Resources;
	}

	void MetalShader::processShaderStruct(NS::Array* members) {
		for (int j = 0; j < members->count(); j++) {
			MTL::StructMember* uniform = (MTL::StructMember*)members->object(j);
			STLR_CORE_INFO("    * Member: (Name: {0}, Type: {1})", uniform->name()->utf8String(), MetalShader::MetalShaderDatatypeToString(uniform->dataType()));
		}
	}

	std::string MetalShader::MetalShaderDatatypeToString(MTL::DataType type) {
		switch (type) {
			case MTL::DataType::DataTypeNone: return "None";
			case MTL::DataType::DataTypeStruct: return "Struct";
			case MTL::DataType::DataTypeArray: return "Array";
			case MTL::DataType::DataTypeFloat: return "Float";
			case MTL::DataType::DataTypeFloat2: return "Float2";
			case MTL::DataType::DataTypeFloat3: return "Float3";
			case MTL::DataType::DataTypeFloat4: return "Float4";
			case MTL::DataType::DataTypeFloat2x2: return "Float2x2";
			case MTL::DataType::DataTypeFloat2x3: return "Float2x3";
			case MTL::DataType::DataTypeFloat2x4: return "Float2x4";
			case MTL::DataType::DataTypeFloat3x2: return "Float3x2";
			case MTL::DataType::DataTypeFloat3x3: return "Float3x3";
			case MTL::DataType::DataTypeFloat3x4: return "Float3x4";
			case MTL::DataType::DataTypeFloat4x2: return "Float4x2";
			case MTL::DataType::DataTypeFloat4x3: return "Float4x3";
			case MTL::DataType::DataTypeFloat4x4: return "Float4x4";
			case MTL::DataType::DataTypeHalf: return "Half";
			case MTL::DataType::DataTypeHalf2: return "Half2";
			case MTL::DataType::DataTypeHalf3: return "Half3";
			case MTL::DataType::DataTypeHalf4: return "Half4";
			case MTL::DataType::DataTypeHalf2x2: return "Half2x2";
			case MTL::DataType::DataTypeHalf2x3: return "Half2x3";
			case MTL::DataType::DataTypeHalf2x4: return "Half2x4";
			case MTL::DataType::DataTypeHalf3x2: return "Half3x2";
			case MTL::DataType::DataTypeHalf3x3: return "Half3x3";
			case MTL::DataType::DataTypeHalf3x4: return "Half3x4";
			case MTL::DataType::DataTypeHalf4x2: return "Half4x2";
			case MTL::DataType::DataTypeHalf4x3: return "Half4x3";
			case MTL::DataType::DataTypeHalf4x4: return "Half4x4";
			case MTL::DataType::DataTypeInt: return "Int";
			case MTL::DataType::DataTypeInt2: return "Int2";
			case MTL::DataType::DataTypeInt3: return "Int3";
			case MTL::DataType::DataTypeInt4: return "Int4";
			case MTL::DataType::DataTypeUInt: return "UInt";
			case MTL::DataType::DataTypeUInt2: return "UInt2";
			case MTL::DataType::DataTypeUInt3: return "UInt3";
			case MTL::DataType::DataTypeUInt4: return "UInt4";
			case MTL::DataType::DataTypeShort: return "Short";
			case MTL::DataType::DataTypeShort2: return "Short2";
			case MTL::DataType::DataTypeShort3: return "Short3";
			case MTL::DataType::DataTypeShort4: return "Short4";
			case MTL::DataType::DataTypeUShort: return "UShort";
			case MTL::DataType::DataTypeUShort2: return "UShort2";
			case MTL::DataType::DataTypeUShort3: return "UShort3";
			case MTL::DataType::DataTypeUShort4: return "UShort4";
			case MTL::DataType::DataTypeChar: return "Char";
			case MTL::DataType::DataTypeChar2: return "Char2";
			case MTL::DataType::DataTypeChar3: return "Char3";
			case MTL::DataType::DataTypeChar4: return "Char4";
			case MTL::DataType::DataTypeUChar: return "UChar";
			case MTL::DataType::DataTypeUChar2: return "UChar2";
			case MTL::DataType::DataTypeUChar3: return "UChar3";
			case MTL::DataType::DataTypeUChar4: return "UChar4";
			case MTL::DataType::DataTypeBool: return "Bool";
			case MTL::DataType::DataTypeBool2: return "Bool2";
			case MTL::DataType::DataTypeBool3: return "Bool3";
			case MTL::DataType::DataTypeBool4: return "Bool4";
			case MTL::DataType::DataTypeTexture: return "Texture";
			case MTL::DataType::DataTypeSampler: return "Sampler";
			case MTL::DataType::DataTypePointer: return "Pointer";
			case MTL::DataType::DataTypeR8Unorm: return "R8Unorm";
			case MTL::DataType::DataTypeR8Snorm: return "R8Snorm";
			case MTL::DataType::DataTypeR16Unorm: return "R16Unorm";
			case MTL::DataType::DataTypeR16Snorm: return "R16Snorm";
			case MTL::DataType::DataTypeRG8Unorm: return "RG8Unorm";
			case MTL::DataType::DataTypeRG8Snorm: return "RG8Snorm";
			case MTL::DataType::DataTypeRG16Unorm: return "RG16Unorm";
			case MTL::DataType::DataTypeRG16Snorm: return "RG16Snorm";
			case MTL::DataType::DataTypeRGBA8Unorm: return "RGBA8Unorm";
			case MTL::DataType::DataTypeRGBA8Unorm_sRGB: return "RGBA8Unorm_sRGB";
			case MTL::DataType::DataTypeRGBA8Snorm: return "RGBA8Snorm";
			case MTL::DataType::DataTypeRGBA16Unorm: return "RGBA16Unorm";
			case MTL::DataType::DataTypeRGBA16Snorm: return "RGBA16Snorm";
			case MTL::DataType::DataTypeRGB10A2Unorm: return "RGB10A2Unorm";
			case MTL::DataType::DataTypeRG11B10Float: return "RG11B10Float";
			case MTL::DataType::DataTypeRGB9E5Float: return "RGB9E5Float";
			case MTL::DataType::DataTypeRenderPipeline: return "RenderPipeline";
			case MTL::DataType::DataTypeComputePipeline: return "ComputePipeline";
			case MTL::DataType::DataTypeIndirectCommandBuffer: return "IndirectCommandBuffer";
			case MTL::DataType::DataTypeLong: return "Long";
			case MTL::DataType::DataTypeLong2: return "Long2";
			case MTL::DataType::DataTypeLong3: return "Long3";
			case MTL::DataType::DataTypeLong4: return "Long4";
			case MTL::DataType::DataTypeULong: return "ULong";
			case MTL::DataType::DataTypeULong2: return "ULong2";
			case MTL::DataType::DataTypeULong3: return "ULong3";
			case MTL::DataType::DataTypeULong4: return "ULong4";
			case MTL::DataType::DataTypeVisibleFunctionTable: return "VisibleFunctionTable";
			case MTL::DataType::DataTypeIntersectionFunctionTable: return "IntersectionFunctionTable";
			case MTL::DataType::DataTypePrimitiveAccelerationStructure: return "PrimitiveAccelerationStructure";
			case MTL::DataType::DataTypeInstanceAccelerationStructure: return "InstanceAccelerationStructure";
			default: return "Unknown Datatype";
		}
	}

	std::string MetalShader::MetalShaderTextureTypeToString(MTL::TextureType type) {
		switch (type) {
			case MTL::TextureType::TextureType1D: return "Texture1D";
			case MTL::TextureType::TextureType1DArray: return "Texture1DArray";
			case MTL::TextureType::TextureType2D: return "Texture2D";
			case MTL::TextureType::TextureType2DArray: return "Texture2DArray";
			case MTL::TextureType::TextureType2DMultisample: return "Texture2DMultisample";
			case MTL::TextureType::TextureTypeCube: return "TextureCube";
			case MTL::TextureType::TextureTypeCubeArray: return "TextureCubeArray";
			case MTL::TextureType::TextureType3D: return "Texture3D";
			case MTL::TextureType::TextureType2DMultisampleArray: return "Texture2DMultisampleArray";
			case MTL::TextureType::TextureTypeTextureBuffer: return "TextureextureBuffer";
			default: return "Unknown type";
		}
	}
}

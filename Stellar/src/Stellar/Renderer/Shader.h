#pragma once

#include "Stellar/Core/Core.h"

#include <string>
#include <unordered_map>

namespace Stellar {

	enum class ShaderUniformType
	{
		None = 0, Bool, Int, UInt, Float, Vec2, Vec3, Vec4, Mat3, Mat4
	};

	class ShaderUniform
	{
	public:
		ShaderUniform() = default;
		ShaderUniform(const std::string& name, ShaderUniformType type, uint32_t size, uint32_t offset);

		const std::string& GetName() const { return m_Name; }
		ShaderUniformType GetType() const { return m_Type; }
		uint32_t GetSize() const { return m_Size; }
		uint32_t GetOffset() const { return m_Offset; }

		static const std::string& UniformTypeToString(ShaderUniformType type);
	private:
		std::string m_Name;
		ShaderUniformType m_Type = ShaderUniformType::None;
		uint32_t m_Size = 0;
		uint32_t m_Offset = 0;
	};

	struct ShaderUniformBuffer
	{
		std::string Name;
		uint32_t Index;
		uint32_t BindingPoint;
		uint32_t Size;
		uint32_t RendererID;
		std::vector<ShaderUniform> Uniforms;
	};

	struct ShaderBuffer
	{
		std::string Name;
		uint32_t Size = 0;
		std::unordered_map<std::string, ShaderUniform> Uniforms;
	};
	
	enum class ShaderType {
		None = 0,
		Vertex = 1,
		Fragment = 2
	};

	class STLR_API Shader {
	public:
		static Shader* Create(const std::string& filePath);
		static const std::string ReadFile(const std::string& fileName);

		virtual ~Shader() {};

		virtual const std::string& getName() const { return m_Name; }
	protected:
		Shader(const std::string& filePath);
		const std::string extractName(const std::string& filePath) const;
	protected:
		std::string m_Name;
		std::string m_FilePath;
	};

	class STLR_API ShaderLibrary {
	public:
		ShaderLibrary() {};
		~ShaderLibrary();

		void add(Shader* shader);
		void add(const std::string& name, Shader* shader);
		void load(const std::string& fileName);
		void load(const std::string& name, const std::string& fileName);
		Shader* get(const std::string& name);
	private:
		std::unordered_map<std::string, Shader*> m_Shaders;
	};
}
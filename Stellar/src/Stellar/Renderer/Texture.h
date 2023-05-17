#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/UUID.h"
#include "Stellar/Core/STLRBase.h"
#include "Stellar/Renderer/Image.h"

#include <imgui.h>

#include <string>
#include <utility>
#include <vector>

namespace Stellar {
	using AssetHandle = UUID;

	struct TextureSpecification {
		ImageFormat format = ImageFormat::RGBA;
		uint32_t width = 1;
		uint32_t height = 1;
		bool generateMips = true;
		bool isImGuiTexture = false;
	};

	class Texture2D : public STLR_Base {
	public:
		AssetHandle Handle;
		virtual ~Texture2D() = default;

		std::string getPath() const { return m_Path; }

		virtual uint64_t getHash() const = 0;
		virtual void generateMips() = 0;
		virtual ImTextureID getImGuiTextureID() = 0;
	
		virtual bool operator==(const Texture2D& other) const {
			return Handle == other.Handle;
		}
		
		virtual bool operator!=(const Texture2D& other) const {
			return !(*this == other);
		}

		static STLR_Ptr<Texture2D> Create(const std::string& filePath, const TextureSpecification& spec);
		static STLR_Ptr<Texture2D> Create(const TextureSpecification& spec, const void* data = nullptr);
		static uint32_t GetMipCount(uint32_t width, uint32_t height);
	protected:
		Texture2D() = default;
		explicit Texture2D(std::string path) : m_Path(std::move(path)) {}

		virtual bool loadImage(const std::string& filePath) = 0;
		std::string m_Path = "";
		STLR_Ptr<Image2D> m_Image{};

		ImTextureID m_TextureId;

		bool m_IsImGuiTexture = false;
	};
}
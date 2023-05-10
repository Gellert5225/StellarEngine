#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/UUID.h"
#include "Stellar/Core/STLRBase.h"
#include "Stellar/Renderer/Image.h"

#include <string>
#include <utility>
#include <vector>

namespace Stellar {
	using AssetHandle = UUID;

	class Texture2D : public STLR_Base {
	public:
		AssetHandle Handle;
		virtual ~Texture2D() = default;

		std::string getPath() const { return m_Path; }

		virtual uint64_t getHash() const = 0;

		static STLR_Ptr<Texture2D> Create(const std::string& filePath);
		static STLR_Ptr<Texture2D> Create(ImageFormat format, uint32_t width = 1, uint32_t height = 1, const void* data = nullptr);
	
		virtual bool operator==(const Texture2D& other) const {
			return Handle == other.Handle;
		}
		
		virtual bool operator!=(const Texture2D& other) const {
			return !(*this == other);
		}
	protected:
		Texture2D() = default;
		explicit Texture2D(std::string path) : m_Path(std::move(path)) {}

		virtual bool loadImage(const std::string& filePath) = 0;
		std::string m_Path = "";
		STLR_Ptr<Image2D> m_Image{};
	};
}
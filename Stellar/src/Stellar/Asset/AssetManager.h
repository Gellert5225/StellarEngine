#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/UUID.h"
#include "Stellar/Core/STLRBase.h"
#include "Stellar/Renderer/Texture.h"
#include "Asset.h"
#include "TextureAsset.h"

#include <unordered_map>
#include <filesystem>

namespace Stellar {
	class AssetManager : public STLR_Base {
	public:
		static AssetManager& Get() {
			static AssetManager instance;
			return instance;
		}

		// Load a texture from file
		AssetID LoadTexture(const std::filesystem::path& filePath, const TextureSpecification& spec = {});

		// Get generic asset by ID
		template<typename T>
		STLR_Ptr<T> GetAsset(AssetID id) {
			if (!m_Assets.contains(id)) return nullptr;
			return m_Assets[id].As<T>();
		}

		// Texture-specific convenience methods
		STLR_Ptr<Texture2D> GetTexture(AssetID id);
		STLR_Ptr<TextureAsset> GetTextureAsset(AssetID id);

		// Check if asset is loaded
		bool IsAssetLoaded(AssetID id) const;

		// Unload asset by ID
		void UnloadAsset(AssetID id);

		// Unload all assets
		void Clear();

		// Find asset ID from path
		AssetID GetAssetIDFromPath(const std::filesystem::path& filePath);

	private:
		AssetManager() = default;

		std::unordered_map<AssetID, STLR_Ptr<Asset>> m_Assets;
		std::unordered_map<std::string, AssetID> m_PathToAssetID;
	};
}

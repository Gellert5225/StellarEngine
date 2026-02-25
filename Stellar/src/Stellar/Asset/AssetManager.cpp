#include "stlrpch.h"
#include "AssetManager.h"
#include "Stellar/Core/Log.h"

namespace Stellar {
	AssetID AssetManager::LoadTexture(const std::filesystem::path& filePath, const TextureSpecification& spec) {
		// Check if already loaded
		if (m_PathToAssetID.contains(filePath.string())) {
			AssetID existingId = m_PathToAssetID[filePath.string()];
			STLR_CORE_WARN("Texture already loaded: {0}", filePath.string());
			return existingId;
		}

		// Check if file exists
		if (!std::filesystem::exists(filePath)) {
			STLR_CORE_ERROR("Texture file not found: {0}", filePath.string());
			return 0;
		}

		// Create asset ID
		AssetID AssetID;

		// Create and load texture
		STLR_Ptr<Texture2D> texture = Texture2D::Create(filePath.string(), spec);
		if (!texture) {
			STLR_CORE_ERROR("Failed to load texture: {0}", filePath.string());
			return 0;
		}

		// Create TextureAsset
		STLR_Ptr<TextureAsset> textureAsset = STLR_Ptr<TextureAsset>(new TextureAsset());
		textureAsset->id = AssetID;
		textureAsset->filePath = filePath;
		textureAsset->setTexture(texture);
		textureAsset->setSpecification(spec);
		textureAsset->isLoaded = true;

		// Store in generic map
		m_Assets[AssetID] = textureAsset;
		m_PathToAssetID[filePath.string()] = AssetID;

		STLR_CORE_INFO("Loaded texture: {0}", filePath.string());
		return AssetID;
	}

	STLR_Ptr<Texture2D> AssetManager::GetTexture(AssetID id) {
		auto textureAsset = GetAsset<TextureAsset>(id);
		if (!textureAsset) {
			STLR_CORE_WARN("Texture asset not found with ID: {0}", (uint64_t)id);
			return nullptr;
		}
		return textureAsset->getTexture();
	}

	STLR_Ptr<TextureAsset> AssetManager::GetTextureAsset(AssetID id) {
		return GetAsset<TextureAsset>(id);
	}

	bool AssetManager::IsAssetLoaded(AssetID id) const {
		return m_Assets.contains(id) && m_Assets.at(id)->isLoaded;
	}

	void AssetManager::UnloadAsset(AssetID id) {
		if (!m_Assets.contains(id)) {
			STLR_CORE_WARN("Cannot unload asset: Asset ID not found");
			return;
		}

		auto asset = m_Assets[id];
		m_PathToAssetID.erase(asset->filePath.string());
		m_Assets.erase(id);

		STLR_CORE_INFO("Unloaded asset: {0}", asset->filePath.string());
	}

	void AssetManager::Clear() {
		m_Assets.clear();
		m_PathToAssetID.clear();
		STLR_CORE_INFO("Cleared all assets");
	}

	AssetID AssetManager::GetAssetIDFromPath(const std::filesystem::path& filePath) {
		if (m_PathToAssetID.contains(filePath.string())) {
			return m_PathToAssetID[filePath.string()];
		}
		return 0; // Invalid ID
	}
}

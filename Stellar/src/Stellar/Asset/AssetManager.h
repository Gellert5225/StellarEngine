#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/UUID.h"
#include "Stellar/Core/STLRBase.h"
#include "Stellar/Core/Log.h"
#include "Stellar/Renderer/Texture.h"
#include "Asset.h"
#include "AssetMetadata.h"
#include "AssetRegistry.h"
#include "AssetImporter.h"

#include <unordered_map>
#include <filesystem>

namespace Stellar {
	class AssetManager : public STLR_Base {
	public:
		static AssetManager& Get() {
			static AssetManager instance;
			return instance;
		}

		// Load the asset registry from disk. Call once at startup.
		void init();

		// Register an asset by path, returning a stable handle. Reuses the
		// existing handle if the path is already registered. Does not load
		// the resource into memory.
		AssetID importAsset(const std::filesystem::path& filePath, AssetType type);

		// Convenience: register a texture asset.
		AssetID loadTexture(const std::filesystem::path& filePath);

		// Get an asset by ID, lazily loading (and caching) it via AssetImporter
		// if it isn't already in the runtime cache.
		template<typename T>
		STLR_Ptr<T> getAsset(AssetID id) {
			if (m_LoadedAssets.contains(id))
				return m_LoadedAssets[id].As<T>();

			if (!m_Registry.contains(id)) {
				STLR_CORE_WARN("No metadata for asset ID: {0}", (uint64_t)id);
				return nullptr;
			}

			STLR_Ptr<Asset> asset = AssetImporter::importAsset(m_Registry.get(id));
			if (!asset)
				return nullptr;

			m_LoadedAssets[id] = asset;
			return asset.As<T>();
		}

		// Texture-specific convenience method
		STLR_Ptr<Texture2D> getTexture(AssetID id);

		// Check if an asset is loaded (present in the runtime cache)
		bool isAssetLoaded(AssetID id) const;

		// Evict an asset from the runtime cache (keeps its registry entry)
		void unloadAsset(AssetID id);

		// Evict all loaded assets from memory (keeps the registry)
		void clear();

		// Find an asset ID from its path (0 if not registered)
		AssetID getAssetIDFromPath(const std::filesystem::path& filePath) const;

		const AssetRegistry& getRegistry() const { return m_Registry; }

	private:
		AssetManager() = default;

		void serializeRegistry();
		void deserializeRegistry();

		AssetRegistry m_Registry;                                     // id -> metadata (persisted)
		std::unordered_map<AssetID, STLR_Ptr<Asset>> m_LoadedAssets;  // id -> loaded object (runtime)
		std::unordered_map<std::string, AssetID> m_PathToHandle;      // path -> id (rebuilt on load)

		inline static const std::filesystem::path s_RegistryPath = "Resources/AssetRegistry.stlr";
	};
}

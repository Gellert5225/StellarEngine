#include "stlrpch.h"
#include "AssetManager.h"
#include "Stellar/Core/Log.h"

#define YAML_CPP_API
#include <yaml-cpp/yaml.h>

namespace Stellar {
	void AssetManager::init() {
		deserializeRegistry();
	}

	AssetID AssetManager::importAsset(const std::filesystem::path& filePath, AssetType type) {
		const std::string key = filePath.lexically_normal().generic_string();

		// Already registered -> return the stable handle
		if (m_PathToHandle.contains(key))
			return m_PathToHandle[key];

		if (!std::filesystem::exists(filePath)) {
			STLR_CORE_ERROR("Asset file not found: {0}", key);
			return 0;
		}

		AssetMetadata metadata;
		metadata.id = AssetID();          // fresh, non-zero UUID
		metadata.type = type;
		metadata.filePath = key;          // store the canonical form
		metadata.status = AssetStatus::Ready;

		m_Registry.set(metadata.id, metadata);
		m_PathToHandle[key] = metadata.id;

		serializeRegistry();

		STLR_CORE_INFO("Imported asset: {0} ({1})", key, assetTypeToString(type));
		return metadata.id;
	}

	AssetID AssetManager::loadTexture(const std::filesystem::path& filePath) {
		return importAsset(filePath, AssetType::Texture);
	}

	STLR_Ptr<Texture2D> AssetManager::getTexture(AssetID id) {
		auto texture = getAsset<Texture2D>(id);
		if (!texture) {
			STLR_CORE_WARN("Texture asset not found with ID: {0}", (uint64_t)id);
			return nullptr;
		}
		return texture;
	}

	bool AssetManager::isAssetLoaded(AssetID id) const {
		return m_LoadedAssets.contains(id);
	}

	void AssetManager::unloadAsset(AssetID id) {
		m_LoadedAssets.erase(id);
	}

	void AssetManager::clear() {
		m_LoadedAssets.clear();
		STLR_CORE_INFO("Unloaded all assets");
	}

	AssetID AssetManager::getAssetIDFromPath(const std::filesystem::path& filePath) const {
		auto it = m_PathToHandle.find(filePath.lexically_normal().generic_string());
		if (it != m_PathToHandle.end())
			return it->second;
		return 0;
	}

	void AssetManager::serializeRegistry() {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "AssetRegistry" << YAML::Value << YAML::BeginSeq;
		for (const auto& [id, metadata] : m_Registry) {
			out << YAML::BeginMap;
			out << YAML::Key << "Handle" << YAML::Value << (uint64_t)metadata.id;
			out << YAML::Key << "FilePath" << YAML::Value << metadata.filePath.string();
			out << YAML::Key << "Type" << YAML::Value << assetTypeToString(metadata.type);
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(s_RegistryPath);
		fout << out.c_str();
	}

	void AssetManager::deserializeRegistry() {
		if (!std::filesystem::exists(s_RegistryPath)) {
			STLR_CORE_INFO("No asset registry found at: {0}", s_RegistryPath.string());
			return;
		}

		std::ifstream stream(s_RegistryPath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		auto entries = data["AssetRegistry"];
		if (!entries)
			return;

		for (auto entry : entries) {
			// Normalize on load so any legacy backslash entries self-heal to the
			// canonical form on the next serialize.
			const std::string filePath = std::filesystem::path(entry["FilePath"].as<std::string>()).lexically_normal().generic_string();

			AssetMetadata metadata;
			metadata.id = entry["Handle"].as<uint64_t>();
			metadata.filePath = filePath;
			metadata.type = assetTypeFromString(entry["Type"].as<std::string>());
			metadata.status = AssetStatus::Ready;

			m_Registry.set(metadata.id, metadata);
			m_PathToHandle[filePath] = metadata.id;
		}

		STLR_CORE_INFO("Loaded asset registry ({0} entries)", m_Registry.count());
	}
}

#include "stlrpch.h"
#include "AssetImporter.h"

#include "Stellar/Core/Log.h"

namespace Stellar {
    std::unordered_map<AssetType, Scope<AssetSerializer>> AssetImporter::s_Serializers;

    void AssetImporter::init() {
        s_Serializers.clear();
        s_Serializers[AssetType::Texture] = CreateScope<TextureSerializer>();
    }

    STLR_Ptr<Asset> AssetImporter::importAsset(const AssetMetadata& metadata) {
        auto it = s_Serializers.find(metadata.type);
        if (it == s_Serializers.end()) {
            STLR_CORE_WARN("No serializer for asset type: {0}", assetTypeToString(metadata.type));
            return nullptr;
        }
        return it->second->tryLoadData(metadata);
    }
}

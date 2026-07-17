#pragma once

#include "Stellar/Core/Core.h"
#include "AssetSerializer.h"
#include "AssetTypes.h"

#include <unordered_map>

namespace Stellar {
    // Façade over a registry of per-type serializers. Dispatches asset loading
    // to the AssetSerializer registered for the asset's AssetType.
    class AssetImporter {
    public:
        static void init();
        static STLR_Ptr<Asset> importAsset(const AssetMetadata& metadata);

    private:
        static std::unordered_map<AssetType, Scope<AssetSerializer>> s_Serializers;
    };
}

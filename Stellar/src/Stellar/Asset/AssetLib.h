#pragma once

#include "Asset.h"
#include <unordered_map>

namespace Stellar {
    class AssetLib {
    public:
        STLR_Ptr<Asset>& operator[](const AssetID id);
        STLR_Ptr<Asset>& get(const AssetID id);
        const STLR_Ptr<Asset>& get(const AssetID id) const;

        bool contains(const AssetID id) const;
        size_t remove(const AssetID id);
    private:
        std::unordered_map<AssetID, STLR_Ptr<Asset>> m_AssetLib;
    };
}
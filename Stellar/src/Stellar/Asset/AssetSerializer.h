#pragma once

#include "Stellar/Core/STLRBase.h"
#include "Stellar/Renderer/Texture.h"
#include "Asset.h"
#include "AssetMetadata.h"

namespace Stellar {
    // Base interface for per-type asset loading. One subclass per AssetType,
    // registered with AssetImporter. Add a serialize() direction later, once
    // there are engine-authored asset types (e.g. materials).
    class AssetSerializer {
    public:
        virtual ~AssetSerializer() = default;

        // Load an asset from its metadata. Returns nullptr on failure.
        virtual STLR_Ptr<Asset> tryLoadData(const AssetMetadata& metadata) = 0;
    };

    class TextureSerializer : public AssetSerializer {
    public:
        STLR_Ptr<Asset> tryLoadData(const AssetMetadata& metadata) override;
    };
}

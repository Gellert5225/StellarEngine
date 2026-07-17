#include "stlrpch.h"
#include "AssetSerializer.h"

#include "Stellar/Core/Log.h"

namespace Stellar {
    STLR_Ptr<Asset> TextureSerializer::tryLoadData(const AssetMetadata& metadata) {
        if (!std::filesystem::exists(metadata.filePath)) {
            STLR_CORE_ERROR("Texture file not found: {0}", metadata.filePath.string());
            return nullptr;
        }

        STLR_Ptr<Texture2D> texture = Texture2D::Create(metadata.filePath.string(), {});
        if (!texture) {
            STLR_CORE_ERROR("Failed to load texture: {0}", metadata.filePath.string());
            return nullptr;
        }

        texture->id = metadata.id;
        texture->filePath = metadata.filePath;
        texture->isLoaded = true;
        return texture;
    }
}

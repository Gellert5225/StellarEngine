#pragma once

#include "Stellar/Core/UUID.h"
#include "Stellar/Core/STLRBase.h"
#include "AssetTypes.h"

#include <filesystem>

namespace Stellar {
    using AssetID = UUID;

    class Asset : public STLR_Base {
    public:
        Asset() = default;
        Asset(AssetType type) : type(type) {}
        AssetID id = UUID(); 
        AssetType type = AssetType::None;

        std::filesystem::path filePath;
        bool isLoaded = false;
    };
}
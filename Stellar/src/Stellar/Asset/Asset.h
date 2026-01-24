#pragma once

#include "Stellar/Core/UUID.h"
#include "Stellar/Core/STLRBase.h"
#include "AssetTypes.h"

#include <filesystem>

namespace Stellar {
    using AssetID = UUID;

    class Asset : public STLR_Base {
    public:
        AssetID id = 0;
        AssetType type;

        std::filesystem::path filePath;
        bool isLoaded = false;
    };
}
#pragma once

#include "Asset.h"

namespace Stellar {
    enum class AssetStatus {
		None = 0, Ready = 1, Invalid = 2, Loading = 3
	};

    struct AssetMetadata {
        AssetID id = 0;
        AssetType type;
        AssetStatus status = AssetStatus::None;

        std::filesystem::path filePath;

        bool isLoaded = false;

        bool isValid() const { return id != 0; }
    };
}
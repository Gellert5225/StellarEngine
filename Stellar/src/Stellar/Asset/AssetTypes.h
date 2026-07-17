#pragma once

#include <string_view>

namespace Stellar {
    enum class AssetType {
        None = 0,
        Scene,
        Prefab,
        Mesh,
        Texture,
        Material,
        EnvMap
    };

    inline const char* assetTypeToString(AssetType type) {
        switch (type) {
            case AssetType::None:     return "None";
            case AssetType::Scene:    return "Scene";
            case AssetType::Prefab:   return "Prefab";
            case AssetType::Mesh:     return "Mesh";
            case AssetType::Texture:  return "Texture";
            case AssetType::Material: return "Material";
            case AssetType::EnvMap:   return "EnvMap";
        }
        return "None";
    }

    inline AssetType assetTypeFromString(std::string_view type) {
        if (type == "Scene")    return AssetType::Scene;
        if (type == "Prefab")   return AssetType::Prefab;
        if (type == "Mesh")     return AssetType::Mesh;
        if (type == "Texture")  return AssetType::Texture;
        if (type == "Material") return AssetType::Material;
        if (type == "EnvMap")   return AssetType::EnvMap;
        return AssetType::None;
    }
}

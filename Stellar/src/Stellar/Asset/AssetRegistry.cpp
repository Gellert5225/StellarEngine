#include "stlrpch.h"
#include "AssetRegistry.h"

#include "Stellar/Core/Log.h"

namespace Stellar {
    const AssetMetadata& AssetRegistry::get(const AssetID& id) const {
        STLR_CORE_ASSERT(m_Registry.contains(id));
        return m_Registry.at(id);
    }

    void AssetRegistry::set(const AssetID id, const AssetMetadata& metadata) {
        STLR_CORE_ASSERT(id == metadata.id);
        STLR_CORE_ASSERT(id != 0);
        m_Registry[id] = metadata;
    }
}
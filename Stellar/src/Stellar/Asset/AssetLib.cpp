#include "stlrpch.h"
#include "AssetLib.h"

#include "Stellar/Core/Log.h"

namespace Stellar {
    STLR_Ptr<Asset>& AssetLib::operator[](const AssetID id) {
        return m_AssetLib[id];
    }

    STLR_Ptr<Asset>& AssetLib::get(const AssetID id) {
        STLR_CORE_ASSERT(m_AssetLib.find(id) != m_AssetLib.end());
        return m_AssetLib.at(id);
    }

    const STLR_Ptr<Asset>& AssetLib::get(const AssetID id) const {
        STLR_CORE_ASSERT(m_AssetLib.find(id) != m_AssetLib.end());
        return m_AssetLib.at(id);
    }

    bool AssetLib::contains(const AssetID id) const {
        return m_AssetLib.find(id) != m_AssetLib.end();
    }

    size_t AssetLib::remove(const AssetID id) {
        return m_AssetLib.erase(id);
    }
}
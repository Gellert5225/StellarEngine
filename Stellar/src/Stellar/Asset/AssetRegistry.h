#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include <unordered_map>

namespace Stellar {
    class AssetRegistry {
    public:
        const AssetMetadata& get(const AssetID& handle) const;
        void set(const AssetID id, const AssetMetadata& metadata);

        size_t count() const { return m_Registry.size(); }
        bool contains(const AssetID id) const { return m_Registry.contains(id); }
        size_t remove(const AssetID id) { return m_Registry.erase(id); }
        void clear() {  m_Registry.clear(); }

	    auto begin() { return m_Registry.begin(); }
		auto end() { return m_Registry.end(); }
		auto begin() const { return m_Registry.cbegin(); }
		auto end() const { return m_Registry.cend(); }

    private:
        std::unordered_map<AssetID, AssetMetadata> m_Registry;
    };
}
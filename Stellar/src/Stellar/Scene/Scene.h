#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/Timestep.h"

#include <entt.h>

namespace Stellar {
    class Entity;

    class STLR_API Scene {
    public:
        Scene();
        ~Scene();

        Entity createEntity(const std::string& name = "");

        void onUpdate(Timestep ts);
        void onViewportResize(uint32_t width, uint32_t height);
    private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

        friend class Entity;
    };
}
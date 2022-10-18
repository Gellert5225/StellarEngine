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
    private:
        entt::registry m_Registry;

        friend class Entity;
    };
}
#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/Timestep.h"

#include <entt.h>

namespace Stellar {
    class STLR_API Scene {
    public:
        Scene();
        ~Scene();

        entt::entity createEntity();

        void onUpdate(Timestep ts);
    private:
        entt::registry m_Registry;
    };
}
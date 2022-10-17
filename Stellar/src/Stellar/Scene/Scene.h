#pragma once

#include "Stellar/Core/Timestep.h"

#include <entt.h>

namespace Stellar {
    class Scene {
    public:
        Scene();
        ~Scene();

        void onUpdate(Timestep ts);
    private:
        entt::registry m_Registry;
    };
}
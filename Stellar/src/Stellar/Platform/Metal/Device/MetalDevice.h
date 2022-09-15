#pragma once

#include "Stellar/Core.h"
#include "Stellar/Platform/Metal/MetalAPI.h"

namespace Stellar {
    class STLR_API MetalDevice {
    public:
        static MetalDevice* GetInstance();
        ~MetalDevice();

        void init();
    private:
        static MetalDevice* s_Instance;
    };
}
#pragma once

#include "Stellar/Core.h"
#include "Stellar/Platform/Metal/MetalAPI.h"

namespace Stellar {
    class STLR_API MetalDevice {
    public:
        static MetalDevice* GetInstance();
        ~MetalDevice();

        void init();

        MTL::Device* getDevice();
        MTL::CommandQueue* getCommandQueue();
    private:
        static MetalDevice* s_Instance;

        MTL::Device* m_Device;
        MTL::CommandQueue* m_CommandQueue;
    };
}
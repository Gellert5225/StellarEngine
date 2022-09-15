#include "stlrpch.h"
#include "MetalDevice.h"

namespace Stellar {
    MetalDevice* MetalDevice::s_Instance = nullptr;

    MetalDevice *MetalDevice::GetInstance() {
        if (s_Instance == nullptr)
            s_Instance = new MetalDevice();
        return s_Instance;
    }

    void MetalDevice::init() {

    }
}
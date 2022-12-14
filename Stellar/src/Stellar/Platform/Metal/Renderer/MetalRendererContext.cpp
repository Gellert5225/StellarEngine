#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "stlrpch.h"
#include "MetalRendererContext.h"
#include "Stellar/Platform/Metal/Device/MetalDevice.h"

namespace Stellar {

    MetalRendererContext::MetalRendererContext() {
        MetalDevice::GetInstance()->init();
    }

    MetalRendererContext::~MetalRendererContext() {
        delete MetalDevice::GetInstance();
    }

    std::string MetalRendererContext::getGPUInfo() const {
        return MetalDevice::GetInstance()->getDevice()->name()->utf8String();
    }

    std::string MetalRendererContext::getGraphicsAPI() const {
        return "Metal";
    }
}
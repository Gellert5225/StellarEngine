#include "stlrpch.h"
#include "MetalRendererContext.h"

namespace Stellar {

    MetalRendererContext::MetalRendererContext() {

    }

    MetalRendererContext::~MetalRendererContext() {

    }

    std::string MetalRendererContext::getGPUInfo() const {
        return std::string();
    }

    std::string MetalRendererContext::getGraphicsAPI() const {
        return std::string();
    }
}
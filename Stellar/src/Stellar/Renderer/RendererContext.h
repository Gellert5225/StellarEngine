#pragma once

#include "Stellar/Core.h"

namespace Stellar {
    class STLR_API RendererContext {
    public:
        RendererContext() = default;
        virtual ~RendererContext() = default;
    };
}
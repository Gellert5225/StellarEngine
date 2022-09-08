#pragma once

#include "Stellar/Core.h"

namespace Stellar {
    class STLR_API RendererContext {
    public:
        virtual void swapBuffers() = 0;
    };
}
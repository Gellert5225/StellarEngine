#pragma once

#include "Core.h"

namespace Stellar {

    class STLR_API Input {
    public:
        static bool IsKeyPressed(int keycode) { return s_Instance->isKeypressedImpl(keycode); }

    protected:
        virtual bool isKeypressedImpl(int) = 0;
    private:
        static Input* s_Instance;
    };
}
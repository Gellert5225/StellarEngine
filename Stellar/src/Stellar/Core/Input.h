#pragma once

#include "Core.h"

namespace Stellar {

    class STLR_API Input {
    public:
        static bool IsKeyPressed(int keycode) { return s_Instance->isKeypressedImpl(keycode); }
        static bool isMouseButtonPressed(int button) { return s_Instance->isMouseButtonPressedImpl(button); }
        static int getMouseX() { return s_Instance->getMouseXImpl(); }
        static int getMouseY() { return s_Instance->getMouseYImpl(); }
    protected:
        virtual bool isKeypressedImpl(int);
        virtual bool isMouseButtonPressedImpl(int);
        virtual float getMouseXImpl();
        virtual float getMouseYImpl();
    private:
        static Input* s_Instance;
    };
}
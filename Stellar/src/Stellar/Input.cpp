#include "stlrpch.h"

#include "Input.h"

#include "Application.h"
#include "Log.h"

namespace Stellar {
    class Input* Input::s_Instance = new Input();

    bool Input::isKeypressedImpl(int keycode) {
        auto window = Application::Get().getWindow().getGLFWWindow();
        auto state = glfwGetKey(window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::isMouseButtonPressedImpl(int button) {
        auto window = Application::Get().getWindow().getGLFWWindow();
        auto state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    float Input::getMouseXImpl() {
        auto window = Application::Get().getWindow().getGLFWWindow();
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        return float(x);
    }

    float Input::getMouseYImpl() {
        auto window = Application::Get().getWindow().getGLFWWindow();
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        return float(y);
    }
}
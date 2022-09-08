#include "stlrpch.h"
#include "Window.h"

#include <utility>
#include "Log.h"

#include "Stellar/Events/ApplicationEvent.h"
#include "Stellar/Events/MouseEvent.h"
#include "Stellar/Events/KeyEvent.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

namespace Stellar {
    static void GLFWErrorCallback(int error, const char* description) {
        STLR_CORE_ERROR("GLFW ERROR: Code {0}, {1}", error, description);
    }

    Window* Window::Create(const WindowProperty& property) {
        return new Window(property);
    }

    Window::Window(WindowProperty property) : m_Property(std::move(property)) {

    }

    void Window::init() {
        m_Data.Title = m_Property.title;
        m_Data.Width = m_Property.width;
        m_Data.Height = m_Property.height;

        [[maybe_unused]] int success = glfwInit();
        STLR_CORE_ASSERT(success, "Could not init GLFW")
        glfwSetErrorCallback(GLFWErrorCallback);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        STLR_CORE_INFO("Creating window {0} ({1}, {2})",
                       m_Property.title, m_Property.width, m_Property.height);

        m_Window = glfwCreateWindow((int)m_Property.width,
                                    (int)m_Property.height,
                                    m_Data.Title.c_str(),
                                    nullptr,
                                    nullptr);
        glfwSetWindowUserPointer(m_Window, &m_Data);
        setVsync(true);

        m_Context = new VulkanRendererContext();
        m_SwapChain = new SwapChain();

        // GLFW callbacks
//        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
//            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
//            data.Width = width;
//            data.Height = height;
//
//            WindowResizeEvent event(width, height);
//            data.EventCallback(event);
//        });

        glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            WindowCloseEvent event;
            data.EventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, 
                                        int key, int scanCode, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action) {
                case GLFW_PRESS: {
                    KeyPressedEvent event(key, 0);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    KeyReleasedEvent event(0);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT: { 
                    KeyPressedEvent event(key, 1);
                    data.EventCallback(event);
                    break;
                }
                default: break;
            }
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            KeyTypedEvent event(keycode);
            data.EventCallback(event);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, 
                                                int button, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action) {
                case GLFW_PRESS: {
                    MouseButtonPressedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    MouseButtonReleasedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
                default: break;
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, 
                                            double xOffset, double yOffset) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.EventCallback(event);                         
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseMovedEvent event((float)x, (float)y);
            data.EventCallback(event);
        });
    }

    unsigned int Window::getHeight() const {
        return m_Data.Height;
    }

    unsigned int Window::getWidth() const {
        return m_Data.Width;
    }

    GLFWwindow* Window::getGLFWWindow() const {
        return m_Window;
    }

    void Window::setEventCallback(const EventCallbackFn& callback) {
        m_Data.EventCallback = callback;
    }

    void Window::onUpdate() {
        glfwPollEvents();
    }

    void Window::setVsync(bool enabled) {
        m_Data.VSync = enabled;
    }

    bool Window::isVsync() const {
        return m_Data.VSync;
    }

    void Window::shutDown() {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    Window::~Window() {
        delete m_SwapChain;
        delete m_Context;
        shutDown();
    }

    SwapChain *Window::getSwapChain() const {
        return m_SwapChain;
    }

    void Window::swapBuffers() {
        m_SwapChain->present();
    }
}
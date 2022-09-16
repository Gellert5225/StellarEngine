#include "stlrpch.h"
#include "MetalImGuiLayer.h"

#include <imgui.h>
#include "Stellar/ImGUI/imgui_impl_glfw.h"
#include "Stellar/Platform/Metal/ImGui/imgui_impl_metal.h"
#include "Stellar/Platform/Metal/Device/MetalDevice.h"
#include "Stellar/Platform/Metal/SwapChain/MetalSwapChain.h"

#include "Stellar/Application.h"

void* createLayer(GLFWwindow* window, double width, double height, void* device);
void* nextDrawable(void* layer);

namespace Stellar {

    MetalImGuiLayer::MetalImGuiLayer() {

    }

    MetalImGuiLayer::~MetalImGuiLayer() {

    }

    void MetalImGuiLayer::onAttach() {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        
        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        auto window = Application::Get().getWindow().getGLFWWindow();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplMetal_Init(MetalDevice::GetInstance()->getDevice());

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
    }

    void MetalImGuiLayer::begin() const {
        auto swapChain = (MetalSwapChain*)Application::Get().getWindow().getSwapChain();
    }

    void MetalImGuiLayer::end() const {

    }

    void MetalImGuiLayer::onDetach() {
        Layer::onDetach();
    }

    void MetalImGuiLayer::onImGuiRender() {
        Layer::onImGuiRender();
    }
}
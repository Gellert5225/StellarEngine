#include "stlrpch.h"
#include "MetalImGuiLayer.h"

#include <imgui.h>
#include "Stellar/ImGUI/imgui_impl_glfw.h"
#include "Stellar/Platform/Metal/ImGui/imgui_impl_metal.h"
#include "Stellar/Platform/Metal/Device/MetalDevice.h"
#include "Stellar/Platform/Metal/SwapChain/MetalSwapChain.h"

#include "Stellar/Application.h"

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
        ImGui_ImplGlfw_InitForOther(window, true);
        ImGui_ImplMetal_Init(MetalDevice::GetInstance()->getDevice());

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
    }

    void MetalImGuiLayer::begin() {
        auto swapChain = (MetalSwapChain*)Application::Get().getWindow().getSwapChain();
        auto colorAttachment = swapChain->getRenderPass()->colorAttachments()->object(0);
        colorAttachment->setClearColor({1, 0, 0, 1});
        colorAttachment->setLoadAction(MTL::LoadActionLoad);
        colorAttachment->setStoreAction(MTL::StoreActionStore);
        m_CommandBuffer = MetalDevice::GetInstance()->getCommandQueue()->commandBuffer();
        m_Encoder = m_CommandBuffer->renderCommandEncoder(swapChain->getRenderPass());

        ImGui_ImplMetal_NewFrame(swapChain->getRenderPass());
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void MetalImGuiLayer::end() const {
        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

        auto time = (float)glfwGetTime();
        io.DeltaTime = m_Time > 0.0 ? (time - m_Time) : (1.0f / 60.0f);

        ImGui::Render();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            auto window = Application::Get().getWindow().getGLFWWindow();
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), m_CommandBuffer, m_Encoder);

        m_Encoder->endEncoding();
        m_Encoder->release();
        m_CommandBuffer->commit();
        m_CommandBuffer->release();
    }

    void MetalImGuiLayer::onDetach() {
        ImGui_ImplMetal_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void MetalImGuiLayer::onImGuiRender() {
        static bool show = true;
        ImGui::ShowDemoWindow(&show);
    }
}
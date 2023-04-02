#include "stlrpch.h"
#include "Application.h"

#include "Log.h"
#include "Input.h"

namespace Stellar {
	Application* Application::s_Instance = nullptr;

	Application::Application() {
		STLR_CORE_ASSERT(!s_Instance, "Application already exists")
		s_Instance = this;
		m_Window = Window::Create();
		m_Window->init();
		m_Window->setEventCallback(BIND_EVENT_FN(Application::onEvent));

		Renderer::Init();
		m_ImGuiLayer = ImGuiLayer::Create();
	}

	Application::~Application() {
		STLR_CORE_INFO("Shutting down");

		for (Layer* layer : m_LayerStack) {
			layer->onDetach();
			delete layer;
		}

		Renderer::Shutdown();
	};

	void Application::onEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::onWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::onWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
			(*--it)->onEvent(e);
			if (e.handled)
				break;
		}
	}

	void Application::pushLayer(Layer* layer) {
		m_LayerStack.pushLayer(layer);
		layer->onAttach();
	}

	void Application::pushOverlay(Layer* layer) {
		m_LayerStack.pushOverlay(layer);
		layer->onAttach();
	}

	void Application::run() {
		while (m_Running) {
			auto time = (float)glfwGetTime();
			Timestep timestep{time - m_LastFrameTime};
			m_LastFrameTime = time;

			auto swapChain = m_Window->getSwapChain();
			swapChain->beginFrame();

			for (Layer* layer : m_LayerStack)
				layer->onUpdate(timestep);

			// imGui
			m_ImGuiLayer->begin();
			for (Layer* layer : m_LayerStack)
				layer->onImGuiRender();
			m_ImGuiLayer->end();

			// present
			m_Window->swapBuffers();

			m_Window->onUpdate();
		}
	}

	bool Application::onWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}

	Application::AppInfo Application::getAppInfo() const {
		return {
			"Stellar Engine",
			m_Window->getRendererContext()->getGPUInfo(),
			m_Window->getRendererContext()->getGraphicsAPI()
		};
	}

	bool Application::onWindowResize(WindowResizeEvent& e) {
		const uint32_t width = e.getWidth(), height = e.getHeight();
		if (width == 0 || height == 0) {
			//m_Minimized = true;
			return false;
		}
		//m_Minimized = false;

		m_Window->getSwapChain()->onResize();

		return false;
	}
}

#pragma once

#include "RenderDLLMacro.hpp"
#include "ApplicationStarter.hpp"
#include "Window.hpp"
#include "GraphicsContext.hpp"
#include "ImguiWindowController.hpp"
#include "ImguiContext.hpp"
#include <CommonException.hpp>
#include <memory>

namespace SampleRenderV2
{
	class SAMPLE_RENDER_DLL_COMMAND Application
	{
	public:
		Application();
		~Application();

		void Run();

		static void EnableSingleton(Application* ptr);
		static Application* GetInstance();

		inline GraphicsAPI GetCurrentAPI()
		{
			return m_Starter->GetCurrentAPI();;
		}

	private:
		std::shared_ptr<Window> m_Window;
		std::shared_ptr<GraphicsContext> m_Context;
		std::shared_ptr<ImguiWindowController> m_ImguiWindowController;
		std::shared_ptr<ImguiContext> m_ImguiContext;
		std::unique_ptr<ApplicationStarter> m_Starter;

		static Application* s_AppSingleton;
		static bool s_SingletonEnabled;
	};
}

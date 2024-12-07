#pragma once

#include "RenderDLLMacro.hpp"
#include "Window.hpp"
#include "GraphicsContext.hpp"
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
			return m_CurrentAPI;
		}

	private:
		std::shared_ptr<Window> m_Window;
		std::shared_ptr<GraphicsContext> m_Context;

		static Application* s_AppSingleton;
		static bool s_SingletonEnabled;
		GraphicsAPI m_CurrentAPI;
	};
}

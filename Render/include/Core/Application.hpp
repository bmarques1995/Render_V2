#pragma once

#include "RenderDLLMacro.hpp"
#include "ApplicationStarter.hpp"
#include "Window.hpp"
#include "GraphicsContext.hpp"
#include "ImguiWindowController.hpp"
#include "ImguiContext.hpp"
#include "CSOCompiler.hpp"
#include "SPVCompiler.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
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

		float vBuffer[42] =
		{
			.0f, .5f, .2f, 1.0f, .0f, .0f, 1.0f,
			.5f, -.5f, .2f, .0f, 1.0f, .0f, 1.0f,
			-.5f, -.5f, .2f, .0f, .0f, 1.0f, 1.0f,
			.0f, .4f, .6f, 1.0f, 1.0f, .0f, 1.0f,
			.4f, -.4f, .6f, .0f, 1.0f, 1.0f, 1.0f,
			-.4f, -.4f, .6f, 1.0f, .0f, 1.0f, 1.0f,
		};

		uint32_t iBuffer[6] =
		{
			3,4,5,
			0,1,2,
		};

		std::shared_ptr<Window> m_Window;
		std::shared_ptr<GraphicsContext> m_Context;
		std::shared_ptr<ImguiWindowController> m_ImguiWindowController;
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;

		std::shared_ptr<CSOCompiler> m_CSOCompiler;
		std::shared_ptr<SPVCompiler> m_SPVCompiler;

		std::shared_ptr<ImguiContext> m_ImguiContext;
		std::unique_ptr<ApplicationStarter> m_Starter;

		static Application* s_AppSingleton;
		static bool s_SingletonEnabled;
	};
}

#pragma once

#include "RenderDLLMacro.hpp"
#include "ApplicationStarter.hpp"
#include "Window.hpp"
#include "GraphicsContext.hpp"
#include "ImguiWindowController.hpp"
#include "ImguiContext.hpp"
#include "CopyPipeline.hpp"
#include "CSOCompiler.hpp"
#include "SPVCompiler.hpp"
#include "LayerStack.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include <CommonException.hpp>
#include <memory>
#include <Eigen/Eigen>


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

		inline const std::shared_ptr<Window>& GetWindow() const 
		{ 
			return m_Window;
		}

		std::shared_ptr<CopyPipeline>* GetCopyPipeline();
		/*{
			return &m_CopyPipeline;
		}*/

	protected:

		Eigen::Vector<float, 9> vBuffer[4] =
		{
			{-.5f, -.5f, .2f, 1.0f, .0f, .0f, 1.0f,  0.0f, 1.0f },
			{-.5f, .5f, .2f, .0f, 1.0f, .0f, 1.0f,  0.0f, 0.0f },
			{.5f, -.5f, .2f, .0f, .0f, 1.0f, 1.0f,  1.0f, 1.0f},
			{.5f, .5f, .2f, 1.0f, 1.0f, .0f, 1.0f,  1.0f, 0.0f},
		};

		uint32_t iBuffer[6] =
		{
			3,2,1,
			1,2,0
		};

		struct SmallMVP
		{
			Eigen::Matrix4f model;
		};

		struct CompleteMVP
		{
			Eigen::Matrix4f model;
			Eigen::Matrix4f view;
			Eigen::Matrix4f projection;
			Eigen::Matrix4f fill;
		};

		SmallMVP m_SmallMVP;
		CompleteMVP m_CompleteMVP;

		std::shared_ptr<Window> m_Window;
		std::shared_ptr<GraphicsContext> m_Context;
		std::shared_ptr<ImguiWindowController> m_ImguiWindowController;
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<CopyPipeline> m_CopyPipeline;
		std::shared_ptr<Texture2D> m_Texture1;
		std::shared_ptr<Texture2D> m_Texture2;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;

		std::shared_ptr<CSOCompiler> m_CSOCompiler;
		std::shared_ptr<SPVCompiler> m_SPVCompiler;

		std::shared_ptr<ImguiContext> m_ImguiContext;
		std::unique_ptr<ApplicationStarter> m_Starter;

		static Application* s_AppSingleton;
		static bool s_SingletonEnabled;
		LayerStack m_LayerStack;
	};
}

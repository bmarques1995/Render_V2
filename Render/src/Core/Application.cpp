#include "Application.hpp"
#include "Console.hpp"
#include <imgui/imgui.h>

SampleRenderV2::Application* SampleRenderV2::Application::s_AppSingleton = nullptr;
bool SampleRenderV2::Application::s_SingletonEnabled = false;

SampleRenderV2::Application::Application()
{
	EnableSingleton(this);
	m_Starter.reset(new ApplicationStarter("render.json"));
	m_Window.reset(Window::Instantiate());
	m_Context.reset(GraphicsContext::Instantiate(m_Window.get(), 3));
	m_Window->ConnectResizer(std::bind(&GraphicsContext::WindowResize, m_Context.get(), std::placeholders::_1, std::placeholders::_2));
	
	ImguiContext::StartImgui();

	m_ImguiWindowController.reset(ImguiWindowController::Instantiate(&m_Window));
	m_ImguiContext.reset(ImguiContext::Instantiate(&m_Context));
}

SampleRenderV2::Application::~Application()
{
	m_ImguiContext.reset();
	m_ImguiWindowController.reset();
	ImguiContext::EndImgui();
	m_Context.reset();
	m_Window.reset();
}

void SampleRenderV2::Application::Run()
{
	bool show_demo_window = true;
	while (!m_Window->ShouldClose())
	{
		m_Window->Update();
		if (!m_Window->IsMinimized())
		{
			try
			{
				m_Context->ReceiveCommands();
				m_Context->StageViewportAndScissors();
				m_ImguiWindowController->ReceiveInput();
				{
					m_ImguiContext->ReceiveInput();
					ImguiContext::StartFrame();
					{
						if (show_demo_window)
							ImGui::ShowDemoWindow(&show_demo_window);
						//m_Context->Draw(m_IndexBuffer->GetCount());
					}
					ImguiContext::EndFrame();
					m_ImguiContext->DispatchInput();
				}
				m_Context->DispatchCommands();
				m_Context->Present();
			}
			catch (GraphicsException e)
			{
				Console::CoreError("Caught error: {}", e.what());
				exit(2);
			}
		}
	}
}

void SampleRenderV2::Application::EnableSingleton(Application* ptr)
{
	if (!s_SingletonEnabled)
	{
		s_SingletonEnabled = true;
		s_AppSingleton = ptr;
	}
}

SampleRenderV2::Application* SampleRenderV2::Application::GetInstance()
{
	return s_AppSingleton;
}

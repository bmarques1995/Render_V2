#include "Application.hpp"
#include "Console.hpp"

SampleRenderV2::Application* SampleRenderV2::Application::s_AppSingleton = nullptr;
bool SampleRenderV2::Application::s_SingletonEnabled = false;

SampleRenderV2::Application::Application()
{
	EnableSingleton(this);
	m_CurrentAPI = GraphicsAPI::SAMPLE_RENDER_GRAPHICS_API_VK;
	m_Window.reset(Window::Instantiate());
	m_Context.reset(GraphicsContext::Instantiate(m_Window.get(), 3));
	m_Window->ConnectResizer(std::bind(&GraphicsContext::WindowResize, m_Context.get(), std::placeholders::_1, std::placeholders::_2));
}

SampleRenderV2::Application::~Application()
{
	m_Context.reset();
	m_Window.reset();
}

void SampleRenderV2::Application::Run()
{
	while (!m_Window->ShouldClose())
	{
		m_Window->Update();
		if (!m_Window->IsMinimized())
		{
			try {
				m_Context->ReceiveCommands();
				m_Context->StageViewportAndScissors();
				//m_Context->Draw(m_IndexBuffer->GetCount());
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

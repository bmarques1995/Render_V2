#include "Application.hpp"

#ifdef RENDER_USES_WINDOWS
	#include <windows.h>
#endif

SampleRenderV2::Application::Application()
{
	m_Window.reset(Window::Instantiate());
	//m_Window->ConnectResizer(std::bind(&GraphicsContext::WindowResize, m_Context.get(), std::placeholders::_1, std::placeholders::_2));
}

SampleRenderV2::Application::~Application()
{
	m_Window.reset();
}

void SampleRenderV2::Application::Run()
{
	while (!m_Window->ShouldClose())
	{
		m_Window->Update();
	}
}
